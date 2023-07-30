// ============================================================================================== //
//                                                                                                //
//   This file is part of the ISF Utilities collection.                                           //
//                                                                                                //
//   Author:                                                                                      //
//   Marcel Hasler <mahasler@gmail.com>                                                           //
//                                                                                                //
//   Copyright (c) 2022 - 2023                                                                    //
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
#include <chrono>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------- //

using namespace std::chrono_literals;

// ---------------------------------------------------------------------------------------------- //

struct SerialPortSettings
{
    size_t baudrate = 115200;
};

// ---------------------------------------------------------------------------------------------- //

class SerialPort
{
public:
    static constexpr std::chrono::milliseconds DefaultTimeout = 500ms;

    using Error = std::runtime_error;

public:
    SerialPort(const std::string& port, const SerialPortSettings& settings = {});

    SerialPort(const SerialPort&) = delete;
    SerialPort(SerialPort&& other);

    ~SerialPort();

    auto operator=(const SerialPort& other) = delete;
    auto operator=(SerialPort&& other) -> SerialPort&;

    auto port() const -> std::string { return m_port; }

    void sendData(std::span<const char> data) const;

    auto waitForDataAvailable(std::chrono::milliseconds timeout = DefaultTimeout) const -> bool;
    auto readAllData() const -> std::vector<char>;

    auto getNumberOfBytesAvailable() const -> size_t;

private:
    void move(SerialPort&& other);
    void close();

    void throwSystemError(std::string error, int code = 0) const;

private:
    std::string m_port;

    struct Private;
    std::unique_ptr<Private> d;
};

// ---------------------------------------------------------------------------------------------- //
