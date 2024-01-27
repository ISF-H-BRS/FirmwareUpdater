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

#pragma once

#include "main.h"

// IMPORTANT:
// Remember to update VECT_TAB_OFFSET in system_stm32f4xx.c
// of the firmware project to the correct start offset.

// By default the bootloader assumes a voltage range of 2.7 - 3.6 V.
// Set this to voltage range 1 or 2 if running at lower voltages.
// #define BOOTLOADER_VOLTAGE_RANGE FLASH_VOLTAGE_RANGE_3

namespace Config {
    constexpr const char* BoardName = "YourBoardNameWithoutSpaces";
    constexpr const char* HardwareVersion = "1.0";
    constexpr const char* BootloaderVersion = "1.0";

    constexpr uint32_t RamStartAddress = 0x20000004; // 32 bits used for bootflags
    constexpr uint32_t RamEndAddress   = 0x20020000; // Adjust for actual SRAM size

    constexpr uint32_t FirmwareStartSector = 2; // 2*16 KiB bootloader space, adjust if needed
    constexpr uint32_t FirmwareSectorCount = 3; // 2*16 KiB + 64 KiB, adjust for actual flash size

    constexpr uint32_t FirmwareStartAddress = 0x08008000; // Adjust according to memory map
    constexpr uint32_t FirmwareEndAddress   = 0x08020000 - sizeof(uint32_t); // 32-bit CRC

    constexpr uint32_t ChecksumAddress = FirmwareEndAddress;

}
