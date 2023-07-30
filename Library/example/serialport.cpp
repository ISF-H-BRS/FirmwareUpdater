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

#include "serialport.h"

#if defined(__linux__)
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <asm/termbits.h>
#include <sys/ioctl.h>
#elif defined(_WIN32)
#include <windows.h>
#include <thread>
#endif

#if defined(_WIN32) && !defined(CREATE_WAITABLE_TIMER_HIGH_RESOLUTION)
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

#include <cassert>
#include <cstring>

// ---------------------------------------------------------------------------------------------- //

struct SerialPort::Private
{
#if defined(__linux__)
    int fd = -1;
#elif defined(_WIN32)
    ::HANDLE handle = INVALID_HANDLE_VALUE;
#endif
};

// ---------------------------------------------------------------------------------------------- //

SerialPort::SerialPort(const std::string& port, const SerialPortSettings& settings)
    : m_port(port),
      d(std::make_unique<Private>())
{
#if defined(__linux__)
    const std::string filename = "/dev/" + port;

    d->fd = ::open(filename.c_str(), O_RDWR | O_NOCTTY);

    if (d->fd < 0)
        throwSystemError("Unable to open serial port " + port + ":");

    ::termios2 termios = {};
    ::ioctl(d->fd, TCGETS2, &termios);

    termios.c_iflag = IGNPAR;
    termios.c_oflag = 0;
    termios.c_cflag = BOTHER | CS8 | CLOCAL | CREAD;
    termios.c_lflag = 0;
    termios.c_cc[VMIN] = 0;
    termios.c_cc[VTIME] = 0;
    termios.c_ispeed = settings.baudrate;
    termios.c_ospeed = settings.baudrate;

    ::ioctl(d->fd, TCSETS2, &termios);
    ::ioctl(d->fd, TIOCEXCL);
    ::ioctl(d->fd, TCFLSH, TCIOFLUSH);
#elif defined(_WIN32)
    const std::string filename = "\\\\.\\" + port;

    d->handle = ::CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
                              0, nullptr, OPEN_EXISTING, 0, nullptr);

    if (d->handle == INVALID_HANDLE_VALUE)
        throwSystemError("Unable to open serial port " + port + ":");

    ::DCB dcb = {};
    ::GetCommState(d->handle, &dcb);

    dcb.BaudRate     = settings.baudrate;
    dcb.fBinary      = TRUE;
    dcb.fParity      = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl  = DTR_CONTROL_DISABLE;
    dcb.fRtsControl  = RTS_CONTROL_DISABLE;
    dcb.ByteSize     = 8;
    dcb.Parity       = NOPARITY;
    dcb.StopBits     = ONESTOPBIT;

    ::COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout         = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant    = 200;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 200;

    ::SetCommState(d->handle, &dcb);
    ::SetCommTimeouts(d->handle, &timeouts);

    ::PurgeComm(d->handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
#endif
}

// ---------------------------------------------------------------------------------------------- //

SerialPort::SerialPort(SerialPort&& other)
    : d(std::make_unique<Private>())
{
    move(std::move(other));
}

// ---------------------------------------------------------------------------------------------- //

SerialPort::~SerialPort()
{
    close();
}

// ---------------------------------------------------------------------------------------------- //

auto SerialPort::operator=(SerialPort&& other) -> SerialPort&
{
    close();
    move(std::move(other));

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

void SerialPort::sendData(std::span<const char> data) const
{
#if defined(__linux__)
    const ssize_t count = ::write(d->fd, data.data(), data.size());
    const bool success = count >= 0 && static_cast<size_t>(count) == data.size();
#elif defined(_WIN32)
    ::DWORD count;
    const bool success = ::WriteFile(d->handle, data.data(), data.size(), &count, nullptr) &&
                         count == data.size();
#endif

    if (!success)
        throwSystemError("Unable to write to serial port:");
}

// ---------------------------------------------------------------------------------------------- //

auto SerialPort::waitForDataAvailable(std::chrono::milliseconds timeout) const -> bool
{
    if (timeout.count() == 0)
        return getNumberOfBytesAvailable() > 0;

#if defined(__linux__)
    ::pollfd pfd = { d->fd, POLLIN, 0 };

    int result = 0;

    do {
        result = ::poll(&pfd, 1, static_cast<int>(timeout.count()));
    } while (result < 0 && errno == EINTR);

    if (result < 0)
        throwSystemError("Unable to wait for data on serial port:");

    return (pfd.revents & POLLIN) != 0;
#elif defined(_WIN32)
    using SteadyClock = std::chrono::steady_clock;
    std::chrono::time_point<SteadyClock> end = SteadyClock::now() + timeout;

    ::HANDLE timer = ::CreateWaitableTimerEx(nullptr, nullptr,
                                             CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                             TIMER_ALL_ACCESS);

    ::LARGE_INTEGER time;
    time.QuadPart = -10000LL; // 1 ms (units of 100 ns)

    bool dataAvailable = false;

    while (SteadyClock::now() < end)
    {
        ::SetWaitableTimer(timer, &time, 0, nullptr, nullptr, false);
        ::WaitForSingleObject(timer, INFINITE);

        dataAvailable = getNumberOfBytesAvailable() > 0;

        if (dataAvailable)
            break;
    }

    ::CloseHandle(timer);
    return dataAvailable;
#endif
}

// ---------------------------------------------------------------------------------------------- //

auto SerialPort::readAllData() const -> std::vector<char>
{
    std::vector<char> data(getNumberOfBytesAvailable());

    if (data.size() > 0)
    {
#if defined(__linux__)
        const ssize_t count = ::read(d->fd, data.data(), data.size());
        const bool success = count >= 0 && static_cast<size_t>(count) == data.size();
#elif defined(_WIN32)
        ::DWORD count;
        const bool success = ::ReadFile(d->handle, data.data(), data.size(), &count, nullptr) &&
                             count == data.size();
#endif

        if (!success)
            throwSystemError("Unable to read from serial port:");
    }

    return data;
}

// ---------------------------------------------------------------------------------------------- //

auto SerialPort::getNumberOfBytesAvailable() const -> size_t
{
#if defined(__linux__)
    int size = 0;

    if (::ioctl(d->fd, FIONREAD, &size) < 0)
        throwSystemError("Unable to get size of available data from serial port:");

    return static_cast<size_t>(size);
#elif defined(_WIN32)
    ::DWORD errors = 0;
    ::COMSTAT stat = {};

    if (!::ClearCommError(d->handle, &errors, &stat))
        throwSystemError("Unable to get size of available data from serial port:");

    return static_cast<size_t>(stat.cbInQue);
#endif
}

// ---------------------------------------------------------------------------------------------- //

void SerialPort::move(SerialPort&& other)
{
    m_port = std::move(other.m_port);

#if defined(__linux__)
    d->fd = other.d->fd;
    other.d->fd = -1;
#elif defined(_WIN32)
    d->handle = other.d->handle;
    other.d->handle = INVALID_HANDLE_VALUE;
#endif
}

// ---------------------------------------------------------------------------------------------- //

void SerialPort::close()
{
#if defined(__linux__)
    if (d->fd >= 0)
        ::close(d->fd);
#elif defined(_WIN32)
    if (d->handle != INVALID_HANDLE_VALUE)
        ::CloseHandle(d->handle);
#endif
}

// ---------------------------------------------------------------------------------------------- //

void SerialPort::throwSystemError(std::string error, int code) const
{
#if defined(__linux__)
    if (code == 0)
        code = errno;

    error = error + " " + std::string(::strerror(code)) + ".";
#elif defined(_WIN32)
    if (code == 0)
        code = ::GetLastError();

    const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS;
    LPSTR buffer = nullptr;

    ::FormatMessageA(flags, nullptr, code, 0, reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

    error = error + " " + std::string(buffer) + ".";

    ::LocalFree(buffer);
#endif

    throw Error(error);
};

// ---------------------------------------------------------------------------------------------- //
