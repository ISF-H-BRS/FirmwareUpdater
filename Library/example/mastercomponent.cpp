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

#include "mastercomponent.h"

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

MasterComponent::MasterComponent(Device* device)
    : m_device(device)
{
    assert(device != nullptr);
}

// ---------------------------------------------------------------------------------------------- //

auto MasterComponent::getBootMode() const -> BootMode
{
    const Device::BootMode mode = m_device->getMasterBootMode();

    if (mode == Device::BootMode::Bootloader)
        return BootMode::Bootloader;

    return BootMode::Firmware;
}

// ---------------------------------------------------------------------------------------------- //

auto MasterComponent::getBootloaderInfo() const -> BootloaderInfo
{
    const Device::BootloaderInfo info = m_device->getMasterBootloaderInfo();

    return {
        info.boardName,
        info.hardwareVersion,
        info.bootloaderVersion,
        info.sectorCount,
        info.firmwareValid
    };
}

// ---------------------------------------------------------------------------------------------- //

auto MasterComponent::getFirmwareInfo() const -> FirmwareInfo
{
    const Device::FirmwareInfo info = m_device->getMasterFirmwareInfo();

    return {
        info.boardName,
        info.hardwareVersion,
        info.firmwareVersion
    };
}

// ---------------------------------------------------------------------------------------------- //

void MasterComponent::launchBootloader()
{
    m_device->launchMasterBootloader();
}

// ---------------------------------------------------------------------------------------------- //

void MasterComponent::launchFirmware()
{
    m_device->launchMasterFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void MasterComponent::unlockFirmware()
{
    m_device->unlockMasterFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void MasterComponent::lockFirmware()
{
    m_device->lockMasterFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void MasterComponent::eraseSector(size_t sector)
{
    m_device->eraseMasterSector(sector);
}

// ---------------------------------------------------------------------------------------------- //

void MasterComponent::writeHexRecord(const std::string& record)
{
    m_device->writeMasterHexRecord(record);
}

// ---------------------------------------------------------------------------------------------- //
