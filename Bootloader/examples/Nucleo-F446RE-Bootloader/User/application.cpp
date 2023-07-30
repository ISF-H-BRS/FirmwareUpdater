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

#include "application.h"
#include "main.h"

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr char TokenSeparator = ' ';
}

// ---------------------------------------------------------------------------------------------- //

Application::Application()
    : m_hostInterface(this) {}

// ---------------------------------------------------------------------------------------------- //

void Application::exec()
{
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);

    while (true)
        m_hostInterface.update();
}

// ---------------------------------------------------------------------------------------------- //

void Application::onHostDataReceived(const String& data)
{
    const size_t tokenCount = data.getTokenCount(TokenSeparator);

    if (tokenCount < 1)
        return;

    const String tag = data.getToken(TokenSeparator, 0);

    if (tag == "<GET_BOOT_MODE>")
        protocolGetBootMode();
    else if (tag == "<GET_BOARD_NAME>")
        protocolGetBoardName();
    else if (tag == "<GET_HARDWARE_VERSION>")
        protocolGetHardwareVersion();
    else if (tag == "<GET_BOOTLOADER_VERSION>")
        protocolGetBootloaderVersion();
    else if (tag == "<GET_SECTOR_COUNT>")
        protocolGetSectorCount();
    else if (tag == "<GET_FIRMWARE_VALID>")
        protocolGetFirmwareValid();
    else if (tag == "<LAUNCH_FIRMWARE>")
        protocolLaunchFirmware();
    else if (tag == "<UNLOCK_FIRMWARE>")
        protocolUnlockFirmware();
    else if (tag == "<LOCK_FIRMWARE>")
        protocolLockFirmware();
    else if (tag == "<ERASE_SECTOR>" || tag == "<WRITE_HEX_RECORD>")
    {
        if (tokenCount < 2)
            return sendError("MISSING_PARAMETER");

        if (tag == "<ERASE_SECTOR>")
        {
            const String sector = data.getToken(TokenSeparator, 1);
            protocolEraseSector(sector);
        }
        else if (tag == "<WRITE_HEX_RECORD>")
        {
            const String record = data.getToken(TokenSeparator, 1);
            protocolWriteHexRecord(record);
        }
    }
    else
        sendError("UNKNOWN_COMMAND");
}

// ---------------------------------------------------------------------------------------------- //

void Application::onHostDataOverflow()
{
    sendError("DATA_OVERFLOW");
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolGetBootMode()
{
    sendResponse("<BOOT_MODE> BOOTLOADER");
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolGetBoardName()
{
    sendResponse("<BOARD_NAME>", Config::BoardName);
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolGetHardwareVersion()
{
    sendResponse("<HARDWARE_VERSION>", Config::HardwareVersion);
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolGetBootloaderVersion()
{
    sendResponse("<BOOTLOADER_VERSION>", Config::BootloaderVersion);
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolGetSectorCount()
{
    sendResponse("<SECTOR_COUNT>", String::makeFormat("%d", Config::FirmwareSectorCount));
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolGetFirmwareValid()
{
    const char* valid = m_bootloader.getFirmwareValid() ? "1" : "0";
    sendResponse("<FIRMWARE_VALID>", valid);
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolLaunchFirmware()
{
    if (!m_bootloader.getFirmwareValid())
        return sendError("INVALID_FIRMWARE");

    sendResponse("<OK>");

    HAL_Delay(500); // Give host time to receive response and disconnect
    m_bootloader.launchFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolUnlockFirmware()
{
    try {
        m_bootloader.unlockFirmware();
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolLockFirmware()
{
    try {
        m_bootloader.lockFirmware();
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolEraseSector(const String& data)
{
    const auto sector = data.toULong();

    try {
        m_bootloader.eraseSector(sector);
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolWriteHexRecord(const String& data)
{
    try {
        m_bootloader.writeHexRecord(data.c_str());
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Application::sendResponse(const String& tag, const String& data)
{
    auto response = tag;

    if (!data.empty())
        response += " " + data;

    m_hostInterface.sendData(response);
}

// ---------------------------------------------------------------------------------------------- //

void Application::sendError(const String& error)
{
    m_hostInterface.sendData("<ERROR> " + error);
}

// ---------------------------------------------------------------------------------------------- //
