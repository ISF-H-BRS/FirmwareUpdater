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

#include "hexrecord.h"

#include <exception>

// ---------------------------------------------------------------------------------------------- //

class Programmer
{
public:
    class EraseError;
    class ProgramError;

public:
    Programmer();
    ~Programmer();

    void eraseSector(size_t sector);
    void processRecord(const HexRecord& record);

private:
    void processExtendedLinearAddress(const HexRecord& record);
    void processData(const HexRecord& record);
    void processEndOfFile(const HexRecord& record);

private:
    uint32_t m_baseAddress = 0;
};

// ---------------------------------------------------------------------------------------------- //

class Programmer::EraseError : public std::exception
{
public:
    enum class Type
    {
        InvalidSector,
        EraseFailed
    };

public:
    EraseError(Type type, uint32_t sector = 0);

    auto type() const -> Type { return m_type; }
    auto sector() const -> uint32_t { return m_sector; }

    auto what() const noexcept -> const char* override;

private:
    Type m_type;
    uint32_t m_sector;
};

// ---------------------------------------------------------------------------------------------- //

class Programmer::ProgramError : public std::exception
{
public:
    enum class Type
    {
        InvalidAddress,
        WriteFailed,
        DataMismatch
    };

public:
    ProgramError(Type type, uint32_t code = 0);

    auto type() const -> Type { return m_type; }
    auto code() const -> uint32_t { return m_code; }

    auto what() const noexcept -> const char* override;

private:
    Type m_type;
    uint32_t m_code;
};

// ---------------------------------------------------------------------------------------------- //
