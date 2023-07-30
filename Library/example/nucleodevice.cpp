// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater library.                                        //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2023                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This library is free software: you can redistribute it and/or modify it under the terms of    //
//  the GNU Lesser General Public License as published by the Free Software Foundation, either    //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU Lesser General Public License for more details.                                   //
//                                                                                                //
//  You should have received a copy of the GNU Lesser General Public License along with this      //
//  library. If not, see <https://www.gnu.org/licenses/>.                                         //
//                                                                                                //
// ============================================================================================== //

#include "nucleodevice.h"

// ---------------------------------------------------------------------------------------------- //

namespace {
    auto split(const std::string& s, char delim) -> std::vector<std::string>
    {
        std::vector<std::string> result;

        std::istringstream stream(s);
        std::string token;

        while (std::getline(stream, token, delim))
            result.push_back(token);

        if (s.back() == delim)
            result.push_back("");

        return result;
    }
}

// ---------------------------------------------------------------------------------------------- //

class InvalidResponseError : public Device::Error
{
public:
    InvalidResponseError(const std::string& response)
        : Device::Error("Invalid response received from device: '" + response + "'") {}
};

// ---------------------------------------------------------------------------------------------- //

Device::Device(const char* port)
    : m_port(port) {}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBootMode() const -> BootMode
{
    const std::string response = sendRequest("<GET_BOOT_MODE>");
    const std::string mode = parseString(response, "<BOOT_MODE>");

    if (mode == "BOOTLOADER")
        return BootMode::Bootloader;

    if (mode == "FIRMWARE")
        return BootMode::Firmware;

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBootloaderInfo() const -> BootloaderInfo
{
    return {
        parseString(sendRequest("<GET_BOARD_NAME>"),         "<BOARD_NAME>"),
        parseString(sendRequest("<GET_HARDWARE_VERSION>"),   "<HARDWARE_VERSION>"),
        parseString(sendRequest("<GET_BOOTLOADER_VERSION>"), "<BOOTLOADER_VERSION>"),
        parseULong( sendRequest("<GET_SECTOR_COUNT>"),       "<SECTOR_COUNT>"),
        parseBool(  sendRequest("<GET_FIRMWARE_VALID>"),     "<FIRMWARE_VALID>")
    };
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getFirmwareInfo() const -> FirmwareInfo
{
    return {
        parseString(sendRequest("<GET_BOARD_NAME>"),       "<BOARD_NAME>"),
        parseString(sendRequest("<GET_HARDWARE_VERSION>"), "<HARDWARE_VERSION>"),
        parseString(sendRequest("<GET_FIRMWARE_VERSION>"), "<FIRMWARE_VERSION>")
    };
}

// ---------------------------------------------------------------------------------------------- //

void Device::launchBootloader()
{
    const std::string response = sendRequest("<LAUNCH_BOOTLOADER>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::launchFirmware()
{
    const std::string response = sendRequest("<LAUNCH_FIRMWARE>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::unlockFirmware()
{
    const std::string response = sendRequest("<UNLOCK_FIRMWARE>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::lockFirmware()
{
    const std::string response = sendRequest("<LOCK_FIRMWARE>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::eraseSector(size_t sector)
{
    const auto timeout = 2s;
    const std::string response = sendRequest("<ERASE_SECTOR> " + std::to_string(sector), timeout);

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::writeHexRecord(const std::string& record)
{
    const auto timeout = 1s;
    const std::string response = sendRequest("<WRITE_HEX_RECORD> " + record, timeout);

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::sendRequest(std::string request,
                         std::chrono::milliseconds timeout) const -> std::string
{
    static constexpr size_t MaximumResponseSize = 64;

    static const std::string lineBreak = "\r\n";

    request += "\r\n";
    m_port.sendData(request);

    std::string response;

    while (true)
    {
        const bool dataAvailable = m_port.waitForDataAvailable(timeout);

        if (!dataAvailable)
            throw Error("Request timed out.");

        const std::vector<char> data = m_port.readAllData();

        if (response.size() + data.size() > MaximumResponseSize)
            throw Error("Invalid response length.");

        for (char c : data)
        {
            response += c;

            if (response.ends_with(lineBreak))
            {
                response.erase(response.size() - lineBreak.size());

                checkError(response);
                return response;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

void Device::checkError(const std::string& response) const
{
    const std::string tag = response.substr(0, response.find_first_of(' '));

    if (tag == "<ERROR>")
    {
        const std::string error = response.substr(tag.length() + 1);
        throw Error(mapError(error));
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseString(const std::string& response,
                         const std::string& expectedTag) const -> std::string
{
    const std::vector<std::string> tokens = ::split(response, ' ');

    if (tokens.size() == 2 && tokens.at(0) == expectedTag)
        return tokens.at(1);

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseULong(const std::string& response,
                        const std::string& expectedTag) const -> unsigned long
{
    const std::string value = parseString(response, expectedTag);

    try {
        return std::stoul(value, nullptr, 0);
    }
    catch (...) {
    }

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseBool(const std::string& response,
                       const std::string& expectedTag) const -> bool
{
    return parseULong(response, expectedTag);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::mapError(const std::string& error) -> std::string
{
    if (error == "DATA_OVERFLOW")
        return "Data overflow.";

    if (error == "UNKNOWN_COMMAND")
        return "Unknown command.";

    if (error == "MISSING_ARGUMENT")
        return "Missing argument.";

    if (error == "INVALID_ARGUMENT")
        return "Invalid argument.";

    if (error == "FIRMWARE_LOCKED")
        return "Firmware locked.";

    if (error == "INVALID_RECORD")
        return "Invalid record.";

    if (error == "INVALID_LENGTH")
        return "Invalid length.";

    if (error == "INVALID_CHECKSUM")
        return "Invalid checksum.";

    if (error == "INVALID_SECTOR")
        return "Invalid sector.";

    if (error == "ERASE_FAILED")
        return "Erase failed.";

    if (error == "INVALID_ADDRESS")
        return "Invalid address.";

    if (error == "WRITE_FAILED")
        return "Write failed.";

    if (error == "DATA_MISMATCH")
        return "Data mismatch.";

    return "Unknown error code received: " + error;
}

// ---------------------------------------------------------------------------------------------- //
