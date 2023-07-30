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

#include <FirmwareUpdater/Core/firmwaremanager.h>
#include <FirmwareUpdater/Core/uploadjob.h>

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

UploadJob::UploadJob(Component* component)
    : m_component(component),
      m_archive(loadArchive(component))
{
    assert(component != nullptr);
}

// ---------------------------------------------------------------------------------------------- //

void UploadJob::run(const ProgressFunction& progress_, const MessageFunction& message_)
{
    static const ProgressFunction dummyProgress = [](int,int,int) {};
    static const MessageFunction dummyMessage = [](const std::string&) {};

    const auto progress = progress_ ? progress_ : dummyProgress;
    const auto message = message_ ? message_ : dummyMessage;

    const Component::BootloaderInfo info = m_component->getBootloaderInfo();

    m_component->unlockFirmware();
    message("Firmware unlocked, erasing now.");

    for (size_t i = 0; i < info.sectorCount; ++i)
    {
        m_component->eraseSector(i);

        message("Sector " + std::to_string(i+1) +
                " of " + std::to_string(info.sectorCount) + " erased.");

        const int eraseProgress = (i+1) * 100 / info.sectorCount;
        progress(eraseProgress, 0, eraseProgress/2);
    }

    const FirmwareArchive::StringList& records = m_archive.hexRecords();

    for (size_t i = 0; i < records.size(); ++i)
    {
        const std::string& record = records.at(i);

        m_component->writeHexRecord(record);

        message("Record " + std::to_string(i+1) +
                " of " + std::to_string(records.size()) + " written.");

        const int uploadProgress = (i+1) * 100 / records.size();
        progress(100, uploadProgress, 50 + uploadProgress/2);
    }

    m_component->lockFirmware();
    message("Firmware upload complete.");
}

// ---------------------------------------------------------------------------------------------- //

auto UploadJob::loadArchive(Component* component) -> FirmwareArchive
{
    assert(component != nullptr);

    const Component::BootloaderInfo info = component->getBootloaderInfo();

    const FirmwareArchive archive =
            FirmwareManager::loadArchive(info.boardName, info.hardwareVersion);

    const FirmwareArchive::Metadata& metadata = archive.metadata();

    if (metadata.boardName != info.boardName)
        throw Error("Board name in firmware metadata doesn't match target hardware.");

    if (metadata.hardwareVersion != info.hardwareVersion)
        throw Error("Hardware version in firmware metadata doesn't match target hardware.");

    return archive;
}

// ---------------------------------------------------------------------------------------------- //
