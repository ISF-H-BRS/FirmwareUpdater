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

#pragma once

#include <FirmwareUpdater/Core/namespace.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

FIRMWAREUPDATER_BEGIN_NAMESPACE();

class FirmwareArchive
{
public:
    struct Metadata
    {
        std::string boardName;
        std::string hardwareVersion;
        std::string firmwareVersion;
        std::string releaseDate;
        std::string packager;
    };

    using StringList = std::vector<std::string>;

    using Error = std::runtime_error;

public:
    FirmwareArchive(const std::string& filename);

    auto metadata() const -> const Metadata&;
    auto hexRecords() const -> const StringList&;

    static void registerPublicKey(const std::string& packager, const std::string& key);

private:
    Metadata m_metadata;
    StringList m_hexRecords;

    static std::map<std::string,std::string> s_publicKeys;
};

FIRMWAREUPDATER_END_NAMESPACE();
