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

#include <memory>

// This is a dummy device class providing just the methods required for using it with the
// Firmware Updater library. It emulates a device consisting of a master and a slave controller.

class Device;
using DevicePtr = std::unique_ptr<Device>;

class Device
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
        // ... other application-specific fields
    };

public:
    auto getMasterBootMode() const -> BootMode { return s_masterBootMode; }

    auto getMasterBootloaderInfo() const -> BootloaderInfo { return s_masterBootloaderInfo; }
    auto getMasterFirmwareInfo() const -> FirmwareInfo { return s_masterFirmwareInfo; }

    void launchMasterBootloader() { s_masterBootMode = BootMode::Bootloader; }
    void launchMasterFirmware() { s_masterBootMode = BootMode::Firmware; }

    void unlockMasterFirmware() {}
    void lockMasterFirmware() {}
    void eraseMasterSector(size_t) {}
    void writeMasterHexRecord(const std::string&) {}

    auto getSlaveBootMode() -> BootMode { return s_slaveBootMode; }

    auto getSlaveBootloaderInfo() -> BootloaderInfo { return s_slaveBootloaderInfo; }
    auto getSlaveFirmwareInfo() -> FirmwareInfo { return s_slaveFirmwareInfo; }

    void launchSlaveBootloader() { s_slaveBootMode = BootMode::Bootloader; }
    void launchSlaveFirmware() { s_slaveBootMode = BootMode::Firmware; }

    void unlockSlaveFirmware() {}
    void lockSlaveFirmware() {}
    void eraseSlaveSector(size_t) {}
    void writeSlaveHexRecord(const std::string&) {}

private:
    static inline BootMode s_masterBootMode = BootMode::Firmware;

    static inline const BootloaderInfo s_masterBootloaderInfo = {
        "DummyMasterBoard", "1.0", "1.0", 4, false
    };

    static inline const FirmwareInfo s_masterFirmwareInfo = {
        "DummyMasterBoard", "1.0", "1.0"
    };

    static inline BootMode s_slaveBootMode = BootMode::Firmware;

    static inline const BootloaderInfo s_slaveBootloaderInfo = {
        "DummySlaveBoard", "2.0", "1.0", 3, true
    };

    static inline const FirmwareInfo s_slaveFirmwareInfo = {
        "DummySlaveBoard", "2.0", "2.0"
    };
};
