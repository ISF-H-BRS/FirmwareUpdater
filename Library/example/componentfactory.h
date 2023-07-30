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

#ifdef FIRMWAREUPDATER_BUILD_DUMMY
#include "dummydevice.h"
#else
#include "nucleodevice.h"
#endif

#include <FirmwareUpdater/Core/componentfactory.h>

class ComponentFactory : public FirmwareUpdater::ComponentFactory
{
public:
#ifdef FIRMWAREUPDATER_BUILD_DUMMY
    static constexpr int MasterUniqueId = 0;
    static constexpr int SlaveUniqueId = 1;
#else
    static constexpr int NucleoUniqueId = 0;
#endif

public:
    ComponentFactory(DevicePtr device);

    auto getComponentList() const -> FirmwareUpdater::ComponentList override;

    auto getDescriptor(int uniqueId) const -> FirmwareUpdater::ComponentDescriptor override;
    auto getComponent(int uniqueId) const -> FirmwareUpdater::ComponentPtr override;

private:
    DevicePtr m_device;
};
