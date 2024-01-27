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

#include "componentfactory.h"

#ifdef FIRMWAREUPDATER_BUILD_DUMMY
#include "mastercomponent.h"
#include "slavecomponent.h"
#else
#include "nucleocomponent.h"
#endif

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

ComponentFactory::ComponentFactory(DevicePtr device)
    : m_device(std::move(device))
{
    assert(m_device != nullptr);
}

// ---------------------------------------------------------------------------------------------- //

#ifdef FIRMWAREUPDATER_BUILD_DUMMY

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getComponentList() const -> FirmwareUpdater::ComponentList
{
    return {
        getDescriptor(MasterUniqueId),
        getDescriptor(SlaveUniqueId)
    };
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getDescriptor(int uniqueId) const -> FirmwareUpdater::ComponentDescriptor
{
    assert(uniqueId == MasterUniqueId || uniqueId == SlaveUniqueId);

    if (uniqueId == MasterUniqueId)
        return { uniqueId, "Master" };

    return { uniqueId, "Slave" };
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getComponent(int uniqueId) const -> FirmwareUpdater::ComponentPtr
{
    assert(uniqueId == MasterUniqueId || uniqueId == SlaveUniqueId);

    if (uniqueId == MasterUniqueId)
        return std::make_unique<MasterComponent>(m_device.get());

    return std::make_unique<SlaveComponent>(m_device.get());
}

// ---------------------------------------------------------------------------------------------- //

#else // !FIRMWAREUPDATER_BUILD_DUMMY

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getComponentList() const -> FirmwareUpdater::ComponentList
{
    return { getDescriptor(NucleoUniqueId) };
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getDescriptor(int uniqueId) const -> FirmwareUpdater::ComponentDescriptor
{
    assert(uniqueId == NucleoUniqueId);
    return { uniqueId, "NucleoF446RE" };
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getComponent(int uniqueId) const -> FirmwareUpdater::ComponentPtr
{
    assert(uniqueId == NucleoUniqueId);
    return std::make_unique<NucleoComponent>(m_device.get());
}

// ---------------------------------------------------------------------------------------------- //

#endif // FIRMWAREUPDATER_BUILD_DUMMY

// ---------------------------------------------------------------------------------------------- //
