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

#include "nucleocomponent.h"

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

NucleoComponent::NucleoComponent(Device* device)
    : m_device(device)
{
    assert(device != nullptr);
}

// ---------------------------------------------------------------------------------------------- //

auto NucleoComponent::getBootMode() const -> BootMode
{
    const Device::BootMode mode = m_device->getBootMode();

    if (mode == Device::BootMode::Bootloader)
        return BootMode::Bootloader;

    return BootMode::Firmware;
}

// ---------------------------------------------------------------------------------------------- //

auto NucleoComponent::getBootloaderInfo() const -> BootloaderInfo
{
    const Device::BootloaderInfo info = m_device->getBootloaderInfo();

    return {
        info.boardName,
        info.hardwareVersion,
        info.bootloaderVersion,
        info.sectorCount,
        info.firmwareValid
    };
}

// ---------------------------------------------------------------------------------------------- //

auto NucleoComponent::getFirmwareInfo() const -> FirmwareInfo
{
    const Device::FirmwareInfo info = m_device->getFirmwareInfo();

    return {
        info.boardName,
        info.hardwareVersion,
        info.firmwareVersion
    };
}

// ---------------------------------------------------------------------------------------------- //

void NucleoComponent::launchBootloader()
{
    m_device->launchBootloader();
}

// ---------------------------------------------------------------------------------------------- //

void NucleoComponent::launchFirmware()
{
    m_device->launchFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void NucleoComponent::unlockFirmware()
{
    m_device->unlockFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void NucleoComponent::lockFirmware()
{
    m_device->lockFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void NucleoComponent::eraseSector(size_t sector)
{
    m_device->eraseSector(sector);
}

// ---------------------------------------------------------------------------------------------- //

void NucleoComponent::writeHexRecord(const std::string& record)
{
    m_device->writeHexRecord(record);
}

// ---------------------------------------------------------------------------------------------- //
