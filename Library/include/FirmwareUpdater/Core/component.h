// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater library.                                        //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2023                                                                     //
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

#include <memory>
#include <string>

FIRMWAREUPDATER_BEGIN_NAMESPACE()

class Component;
using ComponentPtr = std::unique_ptr<Component>;

class Component
{
public:
    enum class BootMode
    {
        Bootloader,
        Firmware
    };

    struct BootloaderInfo
    {
        std::string boardName;
        std::string hardwareVersion;
        std::string bootloaderVersion;
        size_t sectorCount;
        bool firmwareValid;
    };

    struct FirmwareInfo
    {
        std::string boardName;
        std::string hardwareVersion;
        std::string firmwareVersion;
    };

public:
    virtual ~Component() = default;

    virtual auto getBootMode() const -> BootMode = 0;

    virtual auto getBootloaderInfo() const -> BootloaderInfo = 0;
    virtual auto getFirmwareInfo() const -> FirmwareInfo = 0;

    virtual void launchBootloader() = 0;
    virtual void launchFirmware() = 0;

    virtual void unlockFirmware() = 0;
    virtual void lockFirmware() = 0;

    virtual void eraseSector(size_t sector) = 0;
    virtual void writeHexRecord(const std::string& record) = 0;
};

FIRMWAREUPDATER_END_NAMESPACE()
