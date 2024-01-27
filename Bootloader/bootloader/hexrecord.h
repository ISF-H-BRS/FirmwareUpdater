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

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>

class HexRecord
{
public:
    enum class Type : uint8_t
    {
        Data                   = 0,
        EndOfFile              = 1,
        ExtendedSegmentAddress = 2,
        StartSegmentAddress    = 3,
        ExtendedLinearAddress  = 4,
        StartLinearAddress     = 5
    };

    static constexpr size_t TypeCount = 6;

    static constexpr size_t MaximumLength = 32;
    using Data = std::array<uint8_t, MaximumLength>;

    class ParserError;

public:
    auto length() const -> uint8_t;
    auto address() const -> uint16_t;
    auto type() const -> Type;
    auto data() const -> const Data&;
    auto checksum() const -> uint8_t;

    auto computeChecksum() const -> uint8_t;

    static auto fromString(const char* str) -> HexRecord;

private:
    HexRecord() = default;

private:
    uint8_t m_length = 0;
    uint16_t m_address = 0;
    Type m_type = Type::EndOfFile;
    Data m_data = {};
    uint8_t m_checksum = 0;
};

// ---------------------------------------------------------------------------------------------- //

class HexRecord::ParserError : public std::exception
{
public:
    enum class Type
    {
        InvalidRecord,
        InvalidLength,
        InvalidType,
        InvalidChecksum
    };

public:
    ParserError(Type type);

    auto type() const -> Type;
    auto what() const noexcept -> const char* override;

private:
    Type m_type;
};

// ---------------------------------------------------------------------------------------------- //
