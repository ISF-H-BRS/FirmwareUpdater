// ============================================================================================== //
//                                                                                                //
//   This file is part of the ISF Firmware Updater bootloader.                                    //
//                                                                                                //
//   Author:                                                                                      //
//   Marcel Hasler <mahasler@gmail.com>                                                           //
//                                                                                                //
//   Copyright (c) 2020 - 2023                                                                    //
//   Bonn-Rhein-Sieg University of Applied Sciences                                               //
//                                                                                                //
//   Redistribution and use in source and binary forms, with or without modification,             //
//   are permitted provided that the following conditions are met:                                //
//                                                                                                //
//   1. Redistributions of source code must retain the above copyright notice,                    //
//      this list of conditions and the following disclaimer.                                     //
//                                                                                                //
//   2. Redistributions in binary form must reproduce the above copyright notice,                 //
//      this list of conditions and the following disclaimer in the documentation                 //
//      and/or other materials provided with the distribution.                                    //
//                                                                                                //
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"                  //
//   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED            //
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.           //
//   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,             //
//   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT           //
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR           //
//   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,            //
//   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)           //
//   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE                   //
//   POSSIBILITY OF SUCH DAMAGE.                                                                  //
//                                                                                                //
// ============================================================================================== //

#include "bootloader.h"
#include "bootmanager.h"
#include "checksum.h"
#include "config.h"

#include <new>

// ---------------------------------------------------------------------------------------------- //

auto Bootloader::info() const -> const Info&
{
    return m_info;
}

// ---------------------------------------------------------------------------------------------- //

auto Bootloader::getFirmwareValid() const -> bool
{
    return BootManager::getFirmwareValid();
}

// ---------------------------------------------------------------------------------------------- //

void Bootloader::unlockFirmware()
{
    if (!m_programmer)
        m_programmer = new (m_programmerBuffer.data()) Programmer();
}

// ---------------------------------------------------------------------------------------------- //

void Bootloader::lockFirmware()
{
    if (m_programmer)
    {
        m_programmer->~Programmer();
        m_programmer = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------- //

void Bootloader::eraseSector(size_t sector)
{
    if (!m_programmer)
        throw Error(Error::Type::FirmwareLocked);

    m_programmer->eraseSector(sector);
}

// ---------------------------------------------------------------------------------------------- //

void Bootloader::writeHexRecord(const char* string)
{
    if (!m_programmer)
        throw Error(Error::Type::FirmwareLocked);

    const auto record = HexRecord::fromString(string);
    m_programmer->processRecord(record);
}

// ---------------------------------------------------------------------------------------------- //

void Bootloader::launchFirmware()
{
    lockFirmware();
    BootManager::reboot(BootManager::Mode::Firmware);
}

// ---------------------------------------------------------------------------------------------- //

auto Bootloader::Error::what() const noexcept -> const char*
{
    switch (m_type)
    {
    case Type::FirmwareLocked:
        return "FIRMWARE_LOCKED";

    default:
        return "UNKNOWN_BOOTLOADER_ERROR";
    }
}

// ---------------------------------------------------------------------------------------------- //
