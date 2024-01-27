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

#pragma once

#include <FirmwareUpdater/Core/componentfactory.h>
#include <FirmwareUpdater/Core/namespace.h>

#include <QMainWindow>
#include <QToolBar>

#include <memory>

FIRMWAREUPDATER_BEGIN_NAMESPACE();

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~MainWindow() override;

protected:
    explicit MainWindow(QWidget* parent = nullptr);

    auto toolBar() const -> QToolBar*;

    void appendLog(const QString& msg);

    void setConnected(ComponentFactory* factory);
    void setDisconnected();

    virtual auto confirmUploadFirmware(int uniqueId) -> bool;
    virtual void postUploadFirmware(int uniqueId);

    virtual auto confirmLaunchBootloader(int uniqueId) -> bool;
    virtual void postLaunchBootloader(int uniqueId);

    virtual auto confirmLaunchFirmware(int uniqueId) -> bool;
    virtual void postLaunchFirmware(int uniqueId);

protected slots:
    virtual void handleError(const QString& msg);

private slots:
    void uploadFirmware(int uniqueId);
    void launchBootloader(int uniqueId);
    void launchFirmware(int uniqueId);

private:
    void refreshInfo();

private:
    struct Private;
    std::unique_ptr<Private> d;
};

FIRMWAREUPDATER_END_NAMESPACE();
