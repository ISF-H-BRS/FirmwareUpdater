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

#include <FirmwareUpdater/Core/firmwaremanager.h>

#include <filesystem>
#include <regex>

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr const char* RepositoryDirectory = "repository/";
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareManager::getAvailableVersion(const std::string& boardName,
                                          const std::string& hardwareVersion) -> std::string
{
    const std::string base = boardName + "-" + hardwareVersion + "-";
    const std::regex filter(base + "[0-9].[0-9].zip");

    std::string result;

    if (std::filesystem::exists(RepositoryDirectory))
    {
        for (const auto& entry : std::filesystem::directory_iterator(RepositoryDirectory))
        {
            if (entry.is_regular_file())
            {
                const std::string filename = entry.path().filename().string();

                if (std::regex_match(filename, filter))
                {
                    const std::string stem = entry.path().stem().string();
                    const std::string version = stem.substr(base.length());

                    if (version.compare(result) > 0)
                        result = version;
                }
            }
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------- //

auto FirmwareManager::loadArchive(const std::string& boardName,
                                  const std::string& hardwareVersion) -> FirmwareArchive
{
    const std::string firmwareVersion = getAvailableVersion(boardName, hardwareVersion);

    if (firmwareVersion.empty())
        throw Error("No firmware version is available for the given name.");

    const FirmwareArchive archive(RepositoryDirectory + boardName + "-"
                                                      + hardwareVersion + "-"
                                                      + firmwareVersion + ".zip");

    const FirmwareArchive::Metadata& metadata = archive.metadata();

    if (metadata.boardName != boardName)
    {
        throw Error("Board name recorded in firmware metadata doesn't "
                    "match the name specified by the file name.");
    }

    if (metadata.hardwareVersion != hardwareVersion)
    {
        throw Error("Hardware version recorded in firmware metadata doesn't "
                    "match the version specified by the file name.");
    }

    if (metadata.firmwareVersion != firmwareVersion)
    {
        throw Error("Firmware version recorded in firmware metadata doesn't "
                    "match the version specified by the file name.");
    }

    return archive;
}

// ---------------------------------------------------------------------------------------------- //
