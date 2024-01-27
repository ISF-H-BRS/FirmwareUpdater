// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater library.                                        //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2024                                                                     //
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

#include "componentwidget.h"
#include "ui_componentwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

ComponentWidget::ComponentWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::ComponentWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->bootloaderPage, &BootloaderWidget::uploadFirmwareClicked,
            this, &ComponentWidget::uploadFirmwareClicked);

    connect(m_ui->bootloaderPage, &BootloaderWidget::launchFirmwareClicked,
            this, &ComponentWidget::launchFirmwareClicked);

    connect(m_ui->firmwarePage, &FirmwareWidget::launchBootloaderClicked,
            this, &ComponentWidget::launchBootloaderClicked);
}

// ---------------------------------------------------------------------------------------------- //

ComponentWidget::~ComponentWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void ComponentWidget::setBootMode(Component::BootMode mode)
{
    if (mode == Component::BootMode::Bootloader)
        m_ui->stack->setCurrentWidget(m_ui->bootloaderPage);
    else
        m_ui->stack->setCurrentWidget(m_ui->firmwarePage);
}

// ---------------------------------------------------------------------------------------------- //

void ComponentWidget::setBootloaderInfo(const Component::BootloaderInfo& info)
{
    m_ui->bootloaderPage->setInfo(info);
}

// ---------------------------------------------------------------------------------------------- //

void ComponentWidget::setFirmwareInfo(const Component::FirmwareInfo& info)
{
    m_ui->firmwarePage->setInfo(info);
}

// ---------------------------------------------------------------------------------------------- //

void ComponentWidget::setAvailableFirmwareVersion(const std::string& version)
{
    m_ui->bootloaderPage->setAvailableFirmwareVersion(version);
    m_ui->firmwarePage->setAvailableFirmwareVersion(version);
}

// ---------------------------------------------------------------------------------------------- //
