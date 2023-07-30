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

    constexpr uint32_t BootloaderMagic = 0xdeadbeef;
    volatile uint32_t g_bootloaderMagic __attribute__((section(".bootflags")));
}

// ---------------------------------------------------------------------------------------------- //

Application* Application::s_instance = nullptr;

// ---------------------------------------------------------------------------------------------- //

Application::Application()
    : m_hostInterface(this)
{
    s_instance = this;
}

// ---------------------------------------------------------------------------------------------- //

Application::~Application()
{
    s_instance = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void Application::exec()
{
    HAL_TIM_RegisterCallback(Config::LedTimerHandle,
                             HAL_TIM_PERIOD_ELAPSED_CB_ID, &Application::timerElapsedCallback);
    HAL_TIM_Base_Start_IT(Config::LedTimerHandle);

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
    else if (tag == "<GET_FIRMWARE_VERSION>")
        protocolGetFirmwareVersion();
    else if (tag == "<LAUNCH_BOOTLOADER>")
        protocolLaunchBootloader();
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
    sendResponse("<BOOT_MODE> FIRMWARE");
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

void Application::protocolGetFirmwareVersion()
{
    sendResponse("<FIRMWARE_VERSION>", Config::FirmwareVersion);
}

// ---------------------------------------------------------------------------------------------- //

void Application::protocolLaunchBootloader()
{
    sendResponse("<OK>");

    HAL_Delay(500); // Give host time to receive response and disconnect

    g_bootloaderMagic = BootloaderMagic;
    HAL_NVIC_SystemReset();
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

void Application::toggleLed()
{
    m_ledState = !m_ledState;

    GPIO_PinState state = m_ledState ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, state);
}

// ---------------------------------------------------------------------------------------------- //

void Application::timerElapsedCallback(TIM_HandleTypeDef*)
{
    if (s_instance)
        s_instance->toggleLed();
}

// ---------------------------------------------------------------------------------------------- //
