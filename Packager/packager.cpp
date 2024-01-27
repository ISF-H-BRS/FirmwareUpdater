// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater software.                                       //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2024                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This program is free software: you can redistribute it and/or modify it under the terms       //
//  of the GNU General Public License as published by the Free Software Foundation, either        //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU General Public License for more details.                                          //
//                                                                                                //
//  You should have received a copy of the GNU General Public License along with this program.    //
//  If not, see <https://www.gnu.org/licenses/>.                                                  //
//                                                                                                //
// ============================================================================================== //

#include "packager.h"

#include <openssl/evp.h>
#include <openssl/pem.h>

#ifdef __WIN32__
#define ZIP_EXTERN
#endif

#include <zip.h>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

// ---------------------------------------------------------------------------------------------- //

namespace PackagerPrivate {

    using Error = Packager::Error;

    auto openZipFile(const QString& filename) -> zip*;
    void addFileToArchive(zip_t* handle, const QString& filename);
    void createDataArchive(const QString& path);

    int passwordCallback(char* buffer, int size, int rwflag, void* userData);
    void signDataArchive(const QString& path, const QString& signingKey,
                         std::function<QString()> password);

    void createPackage(const QString& path, const Metadata& metadata,
                       const QString& outputDirectory, const Packager::Options& options);
}

// ---------------------------------------------------------------------------------------------- //

using namespace PackagerPrivate;

// ---------------------------------------------------------------------------------------------- //

using ZipGuard = std::unique_ptr<zip, decltype(&zip_close)>;

// ---------------------------------------------------------------------------------------------- //

auto Packager::getOutputFileName(const Metadata& metadata, const Options& options) -> QString
{
    QString filename = metadata.boardName;

    if (options.includeHardwareVersion)
        filename += "-" + metadata.hardwareVersion;

    if (options.includeFirmwareVersion)
        filename += "-" + metadata.firmwareVersion;

    filename += ".zip";
    return filename;
}

// ---------------------------------------------------------------------------------------------- //

void Packager::create(const QString& hexFile, const Metadata& metadata,
                      const QString& signingKey, std::function<QString()> password,
                      const QString& outputDirectory, const Options& options)
{
    QTemporaryDir dir;

    if (!dir.isValid())
        throw Error("Unable to create temporary directory for packaging.");

    const QString path = dir.path() + "/";

    QFile::copy(hexFile, path + "Data.hex");
    writeMetadata(metadata, path + "METADATA");

    createDataArchive(path);
    signDataArchive(path, signingKey, std::move(password));
    createPackage(path, metadata, outputDirectory, options);
}

// ---------------------------------------------------------------------------------------------- //

void Packager::writeMetadata(const Metadata& metadata, const QString& filename)
{
    QFile file(filename);

    if (!file.open(QFile::WriteOnly))
        throw Error("Unable to open temporary metadata file for writing.");

    QTextStream stream(&file);

    stream << "BoardName = "       << metadata.boardName       << "\n";
    stream << "HardwareVersion = " << metadata.hardwareVersion << "\n";
    stream << "FirmwareVersion = " << metadata.firmwareVersion << "\n";
    stream << "ReleaseDate = "     << metadata.releaseDate     << "\n";
    stream << "Packager = "        << metadata.packager        << "\n";
}

// ---------------------------------------------------------------------------------------------- //

auto PackagerPrivate::openZipFile(const QString& filename) -> zip*
{
    int error = 0;
    zip_t* handle = zip_open(filename.toLocal8Bit(), ZIP_CREATE, &error);

    if (!handle)
        throw Error("Unable to create firmware archive.");

    return handle;
}

// ---------------------------------------------------------------------------------------------- //

void PackagerPrivate::addFileToArchive(zip_t* handle, const QString& filename)
{
    const QByteArray name = QFileInfo(filename).fileName().toLocal8Bit();

    zip_error_t error;
    zip_source_t* source = zip_source_file_create(filename.toLocal8Bit(),
                                                  0, ZIP_LENGTH_TO_END, &error);
    if (!source)
        throw Error("Unable to create zip source for file " + name + ".");

    if (zip_file_add(handle, name, source, 0) < 0)
    {
        zip_source_free(source);
        throw Error("Unable to add file " + name + " to archive.");
    }
}

// ---------------------------------------------------------------------------------------------- //

void PackagerPrivate::createDataArchive(const QString& path)
{
    zip_t* handle = openZipFile(path + "Data.zip");
    ZipGuard guard(handle, zip_close);

    addFileToArchive(handle, path + "Data.hex");
    addFileToArchive(handle, path + "METADATA");
}

// ---------------------------------------------------------------------------------------------- //

int PackagerPrivate::passwordCallback(char* buffer, int size, int, void* userData)
{
    auto& getPassword = *static_cast<std::function<QString()>*>(userData);

    QString password = getPassword();

    if (password.size() > size)
        password.truncate(size);

    std::copy_n(password.toLocal8Bit().data(), password.size(), buffer);
    return password.size();
}

// ---------------------------------------------------------------------------------------------- //

void PackagerPrivate::signDataArchive(const QString& path, const QString& signingKey,
                                      std::function<QString()> password)
{
    using PkeyGuard = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
    using CtxGuard = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

    FILE* keyFile = std::fopen(signingKey.toLocal8Bit(), "r");

    if (!keyFile)
        throw Error("Unable to open key file for reading.");

    EVP_PKEY* pkey = PEM_read_PrivateKey(keyFile, nullptr, &passwordCallback, &password);

    if (!pkey)
        throw Error("Unable to read signing key from provided file.");

    PkeyGuard pkeyGuard(pkey, EVP_PKEY_free);
    CtxGuard ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);

    if (EVP_DigestSignInit(ctx.get(), nullptr, EVP_sha256(), nullptr, pkey) <= 0)
        throw Error("Unable to initialize OpenSSL digest signing.");

    QFile dataFile(path + "Data.zip");

    if (!dataFile.open(QFile::ReadOnly))
        throw Error("Unable to open nested data file for reading.");

    const QByteArray data = dataFile.readAll();
    const auto dataPtr = reinterpret_cast<const unsigned char*>(data.data());

    // Get necessary signature size
    size_t siglen = 0;

    if (EVP_DigestSign(ctx.get(), nullptr, &siglen, dataPtr, data.size()) <= 0)
        throw Error("Unable to determine length of signature.");

    std::vector<unsigned char> signature(siglen);

    if (EVP_DigestSign(ctx.get(), signature.data(), &siglen, dataPtr, data.size()) <= 0)
        throw Error("Signature of firmware data failed.");

    std::vector<unsigned char> encodedSignature((siglen/3 + 1) * 4 + 1); // Add byte for '\0'
    EVP_EncodeBlock(encodedSignature.data(), signature.data(), static_cast<int>(signature.size()));

    QFile sigFile(path + "Data.zip.sig");

    if (!sigFile.open(QFile::WriteOnly))
        throw Error("Unable to open signature file for writing.");

    while (encodedSignature.size() > 0)
    {
        auto size = std::min(static_cast<int>(encodedSignature.size()), 65);

        sigFile.write(reinterpret_cast<char*>(encodedSignature.data()), size);
        sigFile.write("\n", 1);

        encodedSignature.erase(encodedSignature.begin(), encodedSignature.begin() + size);
    }
}

// ---------------------------------------------------------------------------------------------- //

void PackagerPrivate::createPackage(const QString& path,
                                    const Metadata& metadata,
                                    const QString& outputDirectory,
                                    const Packager::Options& options)
{
    QString filename = outputDirectory + "/" + Packager::getOutputFileName(metadata, options);

    if (QFile::exists(filename))
        QFile::remove(filename);

    zip_t* handle = openZipFile(filename);
    ZipGuard guard(handle, zip_close);

    addFileToArchive(handle, path + "Data.zip");
    addFileToArchive(handle, path + "Data.zip.sig");

    if (options.saveMetadata)
    {
        filename = outputDirectory + "/METADATA";

        if (QFile::exists(filename))
            QFile::remove(filename);

        QFile::copy(path + "METADATA", filename);
    }
}

// ---------------------------------------------------------------------------------------------- //
