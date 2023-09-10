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

#include <FirmwareUpdater/Core/component.h>
#include <FirmwareUpdater/Core/firmwarearchive.h>

#include <functional>
#include <stdexcept>

FIRMWAREUPDATER_BEGIN_NAMESPACE();

class UploadJob
{
public:
    using ProgressFunction = std::function<void(int erase, int upload, int total)>;
    using MessageFunction = std::function<void(const std::string& msg)>;

    using Error = std::runtime_error;

public:
    UploadJob(Component* component);

    void run(const ProgressFunction& progress = nullptr,
             const MessageFunction& message = nullptr);

private:
    static auto loadArchive(Component* component) -> FirmwareArchive;

private:
    Component* m_component;
    const FirmwareArchive m_archive;
};

FIRMWAREUPDATER_END_NAMESPACE();
