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

#pragma once

#include "componentfactory.h"

#include <FirmwareUpdater/ui.h>

#include <chrono>
using namespace std::chrono_literals;

class MainWindow : public FirmwareUpdater::MainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void openDevice();
    void closeDevice();

    void showAboutDialog();

    void handleError(const QString& msg) override;

private:
    auto confirmLaunchBootloader(int uniqueId) -> bool override;
    void postLaunchBootloader(int uniqueId) override;

    auto confirmLaunchFirmware(int uniqueId) -> bool override;
    void postLaunchFirmware(int uniqueId) override;

#ifdef FIRMWAREUPDATER_BUILD_DUMMY
    auto confirmLaunchMasterBootloader() -> bool;
    auto confirmLaunchSlaveBootloader() -> bool;

    auto confirmLaunchMasterFirmware() -> bool;
    auto confirmLaunchSlaveFirmware() -> bool;
#else
    auto getSerialPort() -> QString;
#endif

    void wait(std::chrono::milliseconds ms, QString label = QString());

private:
    QAction* m_actionConnect = nullptr;
    QAction* m_actionDisconnect = nullptr;
    QAction* m_actionAbout = nullptr;

    std::unique_ptr<ComponentFactory> m_componentFactory;
};
