// ============================================================================================== //
//                                                                                                //
//   This file is part of the ISF Firmware Updater bootloader.                                    //
//                                                                                                //
//   Author:                                                                                      //
//   Marcel Hasler <mahasler@gmail.com>                                                           //
//                                                                                                //
//   Copyright (c) 2020 - 2024                                                                    //
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

#include "bootmanager.h"
#include "checksum.h"
#include "config.h"

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr uint32_t BootloaderMagic = 0xdeadbeef;
    volatile uint32_t g_bootloaderMagic __attribute__((section(".bootflags")));
}

// ---------------------------------------------------------------------------------------------- //

void BootManager::init()
{
    if (g_bootloaderMagic == BootloaderMagic) // Bootloader requested by application
        return;

    if (!getFirmwareValid()) // Don't boot corrupted firmware
        return;

    using FirmwarePtr = void(*)();

    const auto msp = *reinterpret_cast<uint32_t*>(Config::FirmwareStartAddress);
    const auto firmware = *reinterpret_cast<FirmwarePtr*>(Config::FirmwareStartAddress + 4);

    __set_MSP(msp);
    firmware();
}

// ---------------------------------------------------------------------------------------------- //

auto BootManager::getFirmwareValid() -> bool
{
    const auto msp = *reinterpret_cast<uint32_t*>(Config::FirmwareStartAddress);

    if (msp < Config::RamStartAddress || msp > Config::RamEndAddress)
        return false;

    return Checksum::verify();
}

// ---------------------------------------------------------------------------------------------- //

void BootManager::reboot(Mode mode)
{
    g_bootloaderMagic = (mode == Mode::Bootloader) ? BootloaderMagic : 0;
    HAL_NVIC_SystemReset();
}

// ---------------------------------------------------------------------------------------------- //

void boot_manager_init()
{
    BootManager::init();
}

// ---------------------------------------------------------------------------------------------- //
