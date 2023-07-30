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

#include "firmwarewidget.h"
#include "ui_firmwarewidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

FirmwareWidget::FirmwareWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::FirmwareWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->launchBootloaderButton, &QPushButton::clicked,
            this, &FirmwareWidget::launchBootloaderClicked);
}

// ---------------------------------------------------------------------------------------------- //

FirmwareWidget::~FirmwareWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void FirmwareWidget::setInfo(const Component::FirmwareInfo& info)
{
    m_ui->boardName->setText(info.boardName.c_str());
    m_ui->hardwareVersion->setText(info.hardwareVersion.c_str());
    m_ui->currentFirmwareVersion->setText(info.firmwareVersion.c_str());
}

// ---------------------------------------------------------------------------------------------- //

void FirmwareWidget::setAvailableFirmwareVersion(const std::string& version)
{
    m_ui->availableFirmwareVersion->setText(version.empty() ? "None" : version.c_str());

    QString infoText = "Firmware is up to date.";
    bool updateAvailable = false;

    if (version.empty())
        infoText = "No firmware is available for the given hardware version.";
    else
    {
        if (version.compare(m_ui->currentFirmwareVersion->text().toStdString()) > 0)
        {
            infoText = "A newer firmware version is available.";
            updateAvailable = true;
        }
    }

#ifndef NDEBUG
    updateAvailable = true;
#endif

    m_ui->infoLabel->setText(infoText);
    m_ui->launchBootloaderButton->setEnabled(updateAvailable);
}

// ---------------------------------------------------------------------------------------------- //
