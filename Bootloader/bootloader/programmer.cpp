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

#include "checksum.h"
#include "config.h"
#include "programmer.h"

// ---------------------------------------------------------------------------------------------- //

#ifndef BOOTLOADER_VOLTAGE_RANGE
#define BOOTLOADER_VOLTAGE_RANGE FLASH_VOLTAGE_RANGE_3 // 2.7 - 3.6 V, x32 parallelism
#endif

#if BOOTLOADER_VOLTAGE_RANGE == FLASH_VOLTAGE_RANGE_4
#error "FLASH_VOLTAGE_RANGE_4 is not supported."
#endif

// ---------------------------------------------------------------------------------------------- //

namespace {
#if defined(STM32F4)
  #if BOOTLOADER_VOLTAGE_RANGE == FLASH_VOLTAGE_RANGE_1
    using DataType = uint8_t;
    static constexpr uint32_t ProgramType = FLASH_TYPEPROGRAM_BYTE;
  #elif BOOTLOADER_VOLTAGE_RANGE == FLASH_VOLTAGE_RANGE_2
    using DataType = uint16_t;
    static constexpr uint32_t ProgramType = FLASH_TYPEPROGRAM_HALFWORD;
  #else
    using DataType = uint32_t;
    static constexpr uint32_t ProgramType = FLASH_TYPEPROGRAM_WORD;
  #endif
#elif defined(STM32L4)
    using DataType = uint64_t;
    static constexpr uint32_t ProgramType = FLASH_TYPEPROGRAM_DOUBLEWORD;
#endif

    static constexpr uint32_t WordSize = sizeof(DataType);
}

// ---------------------------------------------------------------------------------------------- //

Programmer::Programmer()
{
    HAL_FLASH_Unlock();
}

// ---------------------------------------------------------------------------------------------- //

Programmer::~Programmer()
{
    HAL_FLASH_Lock();
}

// ---------------------------------------------------------------------------------------------- //

void Programmer::eraseSector(size_t sector)
{
    if (sector >= Config::FirmwareSectorCount)
        throw EraseError(EraseError::Type::InvalidSector, sector);

    FLASH_EraseInitTypeDef eraseInit = {};
    uint32_t sectorError = 0;

#if defined(STM32L4)
    eraseInit.TypeErase    = FLASH_TYPEERASE_PAGES;
    eraseInit.Banks        = FLASH_BANK_1;
    eraseInit.Page         = Config::FirmwareStartSector + sector;
    eraseInit.NbPages      = 1;
#elif defined(STM32F4)
    eraseInit.TypeErase    = FLASH_TYPEERASE_SECTORS;
    eraseInit.Sector       = Config::FirmwareStartSector + sector;
    eraseInit.NbSectors    = 1;
    eraseInit.VoltageRange = BOOTLOADER_VOLTAGE_RANGE;
#endif

    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &sectorError);

    if (status != HAL_OK)
        throw EraseError(EraseError::Type::EraseFailed, sectorError);
}

// ---------------------------------------------------------------------------------------------- //

void Programmer::processRecord(const HexRecord& record)
{
    switch (record.type())
    {
    case HexRecord::Type::ExtendedLinearAddress:
        processExtendedLinearAddress(record);
        break;

    case HexRecord::Type::Data:
        processData(record);
        break;

    case HexRecord::Type::EndOfFile:
        processEndOfFile(record);
        break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------------------------------- //

void Programmer::processExtendedLinearAddress(const HexRecord& record)
{
    const HexRecord::Data& bytes = record.data();
    m_baseAddress = (bytes[0] << 24) | (bytes[1] << 16);
}

// ---------------------------------------------------------------------------------------------- //

void Programmer::processData(const HexRecord& record)
{
    const HexRecord::Data& bytes = record.data();

    const uint32_t address = m_baseAddress | record.address();
    const uint32_t length = record.length();

    const bool addressValid = (address % WordSize) == 0 &&
                              (address >= Config::FirmwareStartAddress) &&
                              (address + length <= Config::FirmwareEndAddress);
    if (!addressValid)
        throw ProgramError(ProgramError::Type::InvalidAddress, address);

    for (uint32_t offset = 0; offset < length; offset += WordSize)
    {
        DataType data = 0;

        for (uint32_t i = 0; i < WordSize; ++i)
        {
            const auto byte = static_cast<DataType>(bytes[offset + i]);
            data |= (byte << (i*8));
        }

        auto status = HAL_FLASH_Program(ProgramType, address + offset, data);

        if (status != HAL_OK)
            throw ProgramError(ProgramError::Type::WriteFailed, HAL_FLASH_GetError());

        const auto readback = *reinterpret_cast<volatile DataType*>(address + offset);

        if (readback != data)
            throw ProgramError(ProgramError::Type::DataMismatch);
    }
}

// ---------------------------------------------------------------------------------------------- //

void Programmer::processEndOfFile(const HexRecord&)
{
    const uint32_t checksum = Checksum::compute();

    auto status = HAL_FLASH_Program(ProgramType, Config::ChecksumAddress, checksum);

    if (status != HAL_OK)
        throw ProgramError(ProgramError::Type::WriteFailed, HAL_FLASH_GetError());
}

// ---------------------------------------------------------------------------------------------- //

Programmer::EraseError::EraseError(Type type, uint32_t sector)
    : m_type(type), m_sector(sector) {}

// ---------------------------------------------------------------------------------------------- //

auto Programmer::EraseError::what() const noexcept -> const char*
{
    switch (m_type)
    {
    case Type::InvalidSector:
        return "INVALID_SECTOR";

    case Type::EraseFailed:
        return "ERASE_FAILED";

    default:
        return "UNKNOWN_ERASE_ERROR";
    }
}

// ---------------------------------------------------------------------------------------------- //

Programmer::ProgramError::ProgramError(Type type, uint32_t code)
    : m_type(type), m_code(code) {}

// ---------------------------------------------------------------------------------------------- //

auto Programmer::ProgramError::what() const noexcept -> const char*
{
    switch (m_type)
    {
    case Type::InvalidAddress:
        return "INVALID_ADDRESS";

    case Type::WriteFailed:
        return "WRITE_FAILED";

    case Type::DataMismatch:
        return "DATA_MISMATCH";

    default:
        return "UNKNOWN_PROGRAM_ERROR";
    }
}

// ---------------------------------------------------------------------------------------------- //
