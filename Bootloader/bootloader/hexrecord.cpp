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

#include "hexrecord.h"

#include <cstring>
#include <span>

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::length() const -> uint8_t
{
    return m_length;
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::address() const -> uint16_t
{
    return m_address;
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::type() const -> Type
{
    return m_type;
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::data() const -> const Data&
{
    return m_data;
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::checksum() const -> uint8_t
{
    return m_checksum;
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::computeChecksum() const -> uint8_t
{
    uint8_t checksum = 0x00;

    checksum += m_length;
    checksum += static_cast<uint8_t>(m_address >> 8);
    checksum += static_cast<uint8_t>(m_address);
    checksum += static_cast<uint8_t>(m_type);

    for (uint8_t i = 0; i < m_length; ++i)
        checksum += m_data[i];

    checksum = ~checksum;
    checksum += 1;

    return checksum;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto hexToNum(std::span<const char> hex) -> T
{
    T result = 0;

    for (char c : hex)
    {
        if (c >= '0' && c <= '9')
            c = c - '0';
        else if (c >= 'A' && c <= 'F')
            c = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
            c = c - 'a' + 10;
        else
            return 0;

        result = (result<<4) | c;
    }

    return result;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto scanValue(const char*& str) -> T
{
    std::array<char, 2*sizeof(T)> value = {};

    if (std::strlen(str) < value.size())
        throw HexRecord::ParserError(HexRecord::ParserError::Type::InvalidRecord);

    for (char& byte : value)
        byte = *(str++);

    return hexToNum<T>(value);
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::fromString(const char* str) -> HexRecord
{
    HexRecord record;

    if (std::strlen(str) < 1 || *(str++) != ':')
        throw ParserError(ParserError::Type::InvalidRecord);

    record.m_length = scanValue<uint8_t>(str);

    if (record.m_length > MaximumLength)
        throw ParserError(ParserError::Type::InvalidLength);

    record.m_address = scanValue<uint16_t>(str);

    const uint8_t typeIndex = scanValue<uint8_t>(str);

    if (typeIndex >= TypeCount)
        throw ParserError(ParserError::Type::InvalidType);

    record.m_type = static_cast<Type>(typeIndex);

    for (uint8_t i = 0; i < record.m_length; ++i)
        record.m_data[i] = scanValue<uint8_t>(str);

    record.m_checksum = scanValue<uint8_t>(str);

    if (record.m_checksum != record.computeChecksum())
        throw ParserError(ParserError::Type::InvalidChecksum);

    return record;
}

// ---------------------------------------------------------------------------------------------- //

HexRecord::ParserError::ParserError(Type type)
    : m_type(type) {}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::ParserError::type() const -> Type
{
    return m_type;
}

// ---------------------------------------------------------------------------------------------- //

auto HexRecord::ParserError::what() const noexcept -> const char*
{
    switch (m_type)
    {
    case Type::InvalidRecord:
        return "INVALID_RECORD";

    case Type::InvalidLength:
        return "INVALID_LENGTH";

    case Type::InvalidType:
        return "INVALID_TYPE";

    case Type::InvalidChecksum:
        return "INVALID_CHECKSUM";

    default:
        return "UNKNOWN_PARSER_ERROR";
    }
}

// ---------------------------------------------------------------------------------------------- //
