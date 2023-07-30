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

#include "hostinterface.h"

// ---------------------------------------------------------------------------------------------- //

HostInterface::HostInterface(Owner* owner)
    : m_owner(owner)
{
    HAL_UART_Receive_DMA(m_handle, m_rxBuffer.data(), m_rxBuffer.size());
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::update()
{
    static constexpr const char* LineTerminator = "\r\n";
    static constexpr size_t LineTerminatorLength = 2;

    const size_t size = m_rxBuffer.size() - m_handle->hdmarx->Instance->NDTR;

    for (size_t i = m_currentData.size(); i < size; ++i)
    {
        m_currentData += m_rxBuffer[i];

        if (m_currentData.endsWith(LineTerminator))
        {
            m_currentData.trim(LineTerminatorLength);

            m_owner->onHostDataReceived(m_currentData);
            restartReceive();
        }
        else if (m_handle->hdmarx->Instance->NDTR == 0)
        {
            m_owner->onHostDataOverflow();
            restartReceive();
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::sendData(const String& data)
{
    static const std::array<uint8_t, 2> terminator = { '\r', '\n' };

    std::copy(data.cbegin(), data.cend(), m_txBuffer.begin());
    std::copy(terminator.cbegin(), terminator.cend(), m_txBuffer.begin() + data.size());

    HAL_UART_Transmit_DMA(m_handle, m_txBuffer.data(), data.size() + terminator.size());
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::restartReceive()
{
    m_currentData.clear();

    HAL_UART_AbortReceive(m_handle);
    HAL_UART_Receive_DMA(m_handle, m_rxBuffer.data(), m_rxBuffer.size());
}

// ---------------------------------------------------------------------------------------------- //
