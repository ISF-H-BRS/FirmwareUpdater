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

#include "serialport.h"

#include <memory>
#include <stdexcept>

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
    };

    using Error = std::runtime_error;

public:
    Device(const char* port);

    auto getBootMode() const -> BootMode;

    auto getBootloaderInfo() const -> BootloaderInfo;
    auto getFirmwareInfo() const -> FirmwareInfo;

    void launchBootloader();
    void launchFirmware();

    void unlockFirmware();
    void lockFirmware();
    void eraseSector(size_t);
    void writeHexRecord(const std::string&);

private:
    static constexpr auto DefaultTimeout = SerialPort::DefaultTimeout;

    auto sendRequest(std::string request,
                     std::chrono::milliseconds timeout = DefaultTimeout) const -> std::string;

    void checkError(const std::string& response) const;

    auto parseString(const std::string& response,
                     const std::string& expectedTag) const -> std::string;

    auto parseULong(const std::string& response,
                    const std::string& expectedTag) const -> unsigned long;

    auto parseBool(const std::string& response,
                   const std::string& expectedTag) const -> bool;

    static auto mapError(const std::string& error) -> std::string;

private:
    SerialPort m_port;
};
