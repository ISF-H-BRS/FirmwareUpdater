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

#include "dummydevice.h"

#include <FirmwareUpdater/Core/component.h>

class SlaveComponent : public FirmwareUpdater::Component
{
public:
    SlaveComponent(Device* device);

    auto getBootMode() const -> BootMode override;

    auto getBootloaderInfo() const -> BootloaderInfo override;
    auto getFirmwareInfo() const -> FirmwareInfo override;

    void launchBootloader() override;
    void launchFirmware() override;

    void unlockFirmware() override;
    void lockFirmware() override;

    void eraseSector(size_t sector) override;
    void writeHexRecord(const std::string& record) override;

private:
    Device* m_device;
};
