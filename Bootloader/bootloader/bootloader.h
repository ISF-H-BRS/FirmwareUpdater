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

#pragma once

#include "config.h"
#include "programmer.h"

#include <array>
#include <exception>

class Bootloader
{
public:
    struct Info
    {
        const char* boardName = Config::BoardName;
        const char* hardwareVersion = Config::HardwareVersion;
        const char* bootloaderVersion = Config::BootloaderVersion;
        size_t sectorCount = Config::FirmwareSectorCount;
    };

    class Error;

public:
    Bootloader() = default;
    virtual ~Bootloader() = default;

    auto info() const -> const Info&;

    auto getFirmwareValid() const -> bool;

    void unlockFirmware();
    void lockFirmware();

    void eraseSector(size_t sector);
    void writeHexRecord(const char* record);

    void launchFirmware();

private:
    std::array<char, sizeof(Programmer)> m_programmerBuffer;
    Programmer* m_programmer = nullptr;

    Info m_info;
};

// ---------------------------------------------------------------------------------------------- //

class Bootloader::Error : public std::exception
{
public:
    enum class Type
    {
        FirmwareLocked
    };

public:
    Error(Type type) : m_type(type) {}

    auto type() const -> Type { return m_type; }
    auto what() const noexcept -> const char* override;

private:
    Type m_type;
};

// ---------------------------------------------------------------------------------------------- //
