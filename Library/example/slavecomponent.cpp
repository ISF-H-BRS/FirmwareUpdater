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

#include "slavecomponent.h"

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

SlaveComponent::SlaveComponent(Device* device)
    : m_device(device)
{
    assert(device != nullptr);
}

// ---------------------------------------------------------------------------------------------- //

auto SlaveComponent::getBootMode() const -> BootMode
{
    const Device::BootMode mode = m_device->getSlaveBootMode();

    if (mode == Device::BootMode::Bootloader)
        return BootMode::Bootloader;

    return BootMode::Firmware;
}

// ---------------------------------------------------------------------------------------------- //

auto SlaveComponent::getBootloaderInfo() const -> BootloaderInfo
{
    const Device::BootloaderInfo info = m_device->getSlaveBootloaderInfo();

    return {
        info.boardName,
        info.hardwareVersion,
        info.bootloaderVersion,
        info.sectorCount,
        info.firmwareValid
    };
}

// ---------------------------------------------------------------------------------------------- //

auto SlaveComponent::getFirmwareInfo() const -> FirmwareInfo
{
    const Device::FirmwareInfo info = m_device->getSlaveFirmwareInfo();

    return {
        info.boardName,
        info.hardwareVersion,
        info.firmwareVersion
    };
}

// ---------------------------------------------------------------------------------------------- //#

void SlaveComponent::launchBootloader()
{
    m_device->launchSlaveBootloader();
}

// ---------------------------------------------------------------------------------------------- //

void SlaveComponent::launchFirmware()
{
    m_device->launchSlaveFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void SlaveComponent::unlockFirmware()
{
    m_device->unlockSlaveFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void SlaveComponent::lockFirmware()
{
    m_device->lockSlaveFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void SlaveComponent::eraseSector(size_t sector)
{
    m_device->eraseSlaveSector(sector);
}

// ---------------------------------------------------------------------------------------------- //

void SlaveComponent::writeHexRecord(const std::string& record)
{
    m_device->writeSlaveHexRecord(record);
}

// ---------------------------------------------------------------------------------------------- //
