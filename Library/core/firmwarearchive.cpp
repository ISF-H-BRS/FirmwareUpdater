// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater library.                                        //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2024                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This library is free software: you can redistribute it and/or modify it under the terms of    //
//  the GNU Lesser General Public License as published by the Free Software Foundation, either    //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU Lesser General Public License for more details.                                   //
//                                                                                                //
//  You should have received a copy of the GNU Lesser General Public License along with this      //
//  library. If not, see <https://www.gnu.org/licenses/>.                                         //
//                                                                                                //
// ============================================================================================== //

#include "base64.h"

#include <FirmwareUpdater/Core/firmwarearchive.h>

#include <openssl/evp.h>
#include <openssl/pem.h>

#ifdef __WIN32__
#define ZIP_EXTERN
#endif

#include <zip.h>

#include <algorithm>
#include <memory>
#include <sstream>

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

using PkeyGuard = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using CtxGuard = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

using ZipGuard = std::unique_ptr<zip, decltype(&zip_close)>;

using ByteArray = std::vector<uint8_t>;

using Error = FirmwareArchive::Error;

// ---------------------------------------------------------------------------------------------- //

namespace FirmwareArchivePrivate {

    auto openZipFile(const std::string& filename) -> zip*;
    auto openZipData(const ByteArray& data) -> zip*;

    auto readFile(zip* handle, const std::string& filename) -> ByteArray;

    auto base64Decode(const ByteArray& input) -> ByteArray;
    void verifySignature(const ByteArray& data, const ByteArray& signature, const std::string& key);

    auto parseMetadata(const ByteArray& data) -> FirmwareArchive::Metadata;
    auto parseHexFile(const ByteArray& data) -> FirmwareArchive::StringList;

    auto trimString(const std::string& s) -> std::string;
    auto splitString(const std::string& s, char delim) -> std::vector<std::string>;
}

// ---------------------------------------------------------------------------------------------- //

std::map<std::string,std::string> FirmwareArchive::s_publicKeys;

// ---------------------------------------------------------------------------------------------- //

FirmwareArchive::FirmwareArchive(const std::string& filename)
{
    using namespace FirmwareArchivePrivate;

    zip* fileHandle = openZipFile(filename);
    ZipGuard fileGuard(fileHandle, zip_close);

    const ByteArray dataFile = readFile(fileHandle, "Data.zip");
    const ByteArray signature = readFile(fileHandle, "Data.zip.sig");

    zip* dataHandle = openZipData(dataFile);
    ZipGuard dataGuard(dataHandle, zip_close);

    const ByteArray metadata = readFile(dataHandle, "METADATA");
    m_metadata = parseMetadata(metadata);

    auto it = s_publicKeys.find(m_metadata.packager);

    if (it == s_publicKeys.end())
    {
        throw Error("Unable to verify package signature. No RSA public key "
                    "registered for packager " + m_metadata.packager + ".");
    }

    verifySignature(dataFile, base64Decode(signature), it->second);

    const ByteArray hexFile = readFile(dataHandle, "Data.hex");
    m_hexRecords = parseHexFile(hexFile);
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchive::metadata() const -> const Metadata&
{
    return m_metadata;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchive::hexRecords() const -> const StringList&
{
    return m_hexRecords;
}

// ---------------------------------------------------------------------------------------------- //

void FirmwareArchive::registerPublicKey(const std::string& packager, const std::string& key)
{
    const bool keyValid = key.starts_with("-----BEGIN RSA PUBLIC KEY-----\n") &&
                          key.ends_with("-----END RSA PUBLIC KEY-----\n");
    if (!keyValid)
        throw Error("RSA key not in valid PEM format.");

    s_publicKeys[packager] = key;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::openZipFile(const std::string& filename) -> zip*
{
    int error = 0;

    zip* handle = zip_open(filename.c_str(), ZIP_RDONLY, &error);

    if (!handle)
        throw Error("Unable to open firmware archive \"" + filename + "\".");

    return handle;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::openZipData(const ByteArray& data) -> zip*
{
    const int freep = false; // Don't let libzip free data
    zip_error_t error;

    zip_source_t* src = zip_source_buffer_create(data.data(), data.size(), freep, &error);

    if (src)
    {
        zip* handle = zip_open_from_source(src, 0, &error);

        if (handle)
            return handle;

        zip_source_free(src);
    }

    throw Error("Unable to load compressed firmware data file from archive.");
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::readFile(zip* handle, const std::string& filename) -> ByteArray
{
    zip_flags_t flags = 0;

    struct zip_stat stat = {};
    zip_stat_init(&stat);

    if (zip_stat(handle, filename.c_str(), flags, &stat) < 0)
        throw Error("Unable to find file " + filename + " in firmware archive.");

    zip_file* file = zip_fopen(handle, filename.c_str(), flags);

    if (!file)
        throw Error("Unable to open file " + filename + " from firmware archive.");

    ByteArray buffer(stat.size);

    const zip_int64_t read = zip_fread(file, buffer.data(), stat.size);

    zip_fclose(file);

    if (read != static_cast<zip_int64_t>(stat.size))
        throw Error("Failed to read content of file " + filename + " from firmware archive.");

    return buffer;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::base64Decode(const ByteArray& input) -> ByteArray
{
    ByteArray result(BASE64_DECODE_RESULT_SIZE(input.size()));
    size_t length = 0;

    std::string string;
    std::copy_if(input.begin(), input.end(),
                 std::back_inserter(string), [](char c) { return !std::isspace(c); });

    if (base64_decode(string.c_str(), result.data(), &length) < 0)
        throw Error("Unable to decode Base64 data in firmware archive.");

    result.resize(length);
    return result;
}

// ---------------------------------------------------------------------------------------------- //

void FirmwareArchivePrivate::verifySignature(const ByteArray& data, const ByteArray& signature,
                                             const std::string& key)
{
    BIO* bio = BIO_new_mem_buf(key.data(), static_cast<int>(key.size()));
    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);

    BIO_free_all(bio);

    if (!pkey)
        throw Error("Unable to parse RSA public key.");

    EVP_MD_CTX* ctx = EVP_MD_CTX_create();

    PkeyGuard pkeyGuard(pkey, EVP_PKEY_free);
    CtxGuard ctxGuard(ctx, EVP_MD_CTX_free);

    if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) <= 0)
        throw Error("Unable to initialize OpenSSL digest verification.");

    const int result = EVP_DigestVerify(ctx, signature.data(), signature.size(),
                                             data.data(), data.size());
    if (result < 0)
        throw Error("OpenSSL digest verification failed.");

    if (result != 1)
        throw Error("Firmware package signature verification failed.");
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::parseMetadata(const ByteArray& data) -> FirmwareArchive::Metadata
{
    FirmwareArchive::Metadata metadata;

    std::string string(data.begin(), data.end());
    std::istringstream stream(string);

    std::string line;
    while (std::getline(stream, line))
    {
        const std::vector<std::string> tokens = splitString(line, '=');

        if (tokens.size() != 2)
            throw Error("Invalid line \"" + line + "\" in firmware metadata.");

        const std::string& key = tokens.at(0);
        const std::string& value = tokens.at(1);

        if (key == "BoardName")
            metadata.boardName = value;
        else if (key == "HardwareVersion")
            metadata.hardwareVersion = value;
        else if (key == "FirmwareVersion")
            metadata.firmwareVersion = value;
        else if (key == "ReleaseDate")
            metadata.releaseDate = value;
        else if (key == "Packager")
            metadata.packager = value;
        else
            throw Error("Invalid key \"" + key + "\" in firmware metadata.");
    }

    if (metadata.boardName.empty())
        throw Error("No board name specified in firmware metadata.");

    if (metadata.hardwareVersion.empty())
        throw Error("No hardware version specified in firmware metadata.");

    if (metadata.firmwareVersion.empty())
        throw Error("No firmware version specified in firmware metadata.");

    if (metadata.releaseDate.empty())
        throw Error("No release date specified in firmware metadata.");

    if (metadata.packager.empty())
        throw Error("No packager specified in firmware metadata.");

    return metadata;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::parseHexFile(const ByteArray& data) -> FirmwareArchive::StringList
{
    FirmwareArchive::StringList hexRecords;

    std::string string(data.begin(), data.end());
    std::istringstream stream(string);

    size_t lineNumber = 0;

    std::string line;
    while (std::getline(stream, line))
    {
        ++lineNumber;

        std::erase(line, '\r');
        std::erase(line, '\n');

        if (line.empty())
            continue;

        if (line.front() != ':')
        {
            throw Error("Invalid record in firmware data on line "
                            + std::to_string(lineNumber) + ".");
        }

        hexRecords.push_back(std::move(line));
    }

    return hexRecords;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::trimString(const std::string& s) -> std::string
{
    std::string string = s;

    string.erase(string.begin(),
                 std::find_if(string.begin(), string.end(),
                              [](char c) { return !std::isspace(c); }));

    string.erase(std::find_if(string.rbegin(), string.rend(),
                              [](char c) { return !std::isspace(c); }).base(),
                 string.end());

    return string;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareArchivePrivate::splitString(const std::string& s,
                                         char delim) -> std::vector<std::string>
{
    std::vector<std::string> result;

    std::istringstream stream(s);
    std::string token;

    while (std::getline(stream, token, delim))
        result.push_back(trimString(token));

    return result;
}

// ---------------------------------------------------------------------------------------------- //
