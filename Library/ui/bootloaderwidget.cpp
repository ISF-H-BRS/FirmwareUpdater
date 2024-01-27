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

#include "bootloaderwidget.h"
#include "ui_bootloaderwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

BootloaderWidget::BootloaderWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::BootloaderWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->uploadFirmwareButton, &QPushButton::clicked,
            this, &BootloaderWidget::uploadFirmwareClicked);

    connect(m_ui->launchFirmwareButton, &QPushButton::clicked,
            this, &BootloaderWidget::launchFirmwareClicked);
}

// ---------------------------------------------------------------------------------------------- //

BootloaderWidget::~BootloaderWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void BootloaderWidget::setInfo(const Component::BootloaderInfo& info)
{
    m_ui->boardName->setText(info.boardName.c_str());
    m_ui->hardwareVersion->setText(info.hardwareVersion.c_str());
    m_ui->bootloaderVersion->setText(info.bootloaderVersion.c_str());
    m_ui->currentFirmwareStatus->setText(info.firmwareValid ? "Valid" : "Invalid");

    m_ui->launchFirmwareButton->setEnabled(info.firmwareValid);
}

// ---------------------------------------------------------------------------------------------- //

void BootloaderWidget::setAvailableFirmwareVersion(const std::string& version)
{
    m_ui->availableFirmwareVersion->setText(version.empty() ? "None" : version.c_str());
    m_ui->uploadFirmwareButton->setEnabled(!version.empty());

    const QString infoText =
            version.empty() ? "No firmware is available for the given hardware version."
                            : "A firmware file is ready to upload.";

    m_ui->infoLabel->setText(infoText);
}

// ---------------------------------------------------------------------------------------------- //
