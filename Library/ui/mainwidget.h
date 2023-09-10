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

#include <FirmwareUpdater/Core/componentfactory.h>

#include <QListWidgetItem>
#include <QWidget>

#include <memory>

namespace Ui {
    class MainWidget;
}

FIRMWAREUPDATER_BEGIN_NAMESPACE();

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget() override;

    void setConnected(ComponentFactory* factory);
    void setDisconnected();

public slots:
    void updateComponentInfo();

signals:
    void uploadFirmwareClicked(int uniqueId);
    void launchFirmwareClicked(int uniqueId);
    void launchBootloaderClicked(int uniqueId);

    void error(const QString& msg);

private slots:
    void onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

    void onUploadFirmwareClicked();
    void onLaunchFirmwareClicked();
    void onLaunchBootloaderClicked();

private:
    void updateComponentWidget(int uniqueId);

    auto getCurrentComponentId() const -> int;

private:
    std::unique_ptr<Ui::MainWidget> m_ui;
    ComponentFactory* m_componentFactory = nullptr;
};

FIRMWAREUPDATER_END_NAMESPACE();
