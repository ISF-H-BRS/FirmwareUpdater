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

#include "mainwidget.h"
#include "uploaddialog.h"

#include "ui_mainwindow.h"

#include <FirmwareUpdater/Ui/mainwindow.h>

#include <QMessageBox>

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

struct MainWindow::Private
{
    Ui::MainWindow ui;
    ComponentFactory* componentFactory = nullptr;
};

// ---------------------------------------------------------------------------------------------- //

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      d(std::make_unique<Private>())
{
    d->ui.setupUi(this);

    QFont font("monospace");
    font.setStyleHint(QFont::TypeWriter);
    d->ui.logWidget->setFont(font);

    connect(d->ui.mainWidget, &MainWidget::uploadFirmwareClicked,
            this, &MainWindow::uploadFirmware);

    connect(d->ui.mainWidget, &MainWidget::launchFirmwareClicked,
            this, &MainWindow::launchFirmware);

    connect(d->ui.mainWidget, &MainWidget::launchBootloaderClicked,
            this, &MainWindow::launchBootloader);

    connect(d->ui.mainWidget, &MainWidget::error,
            this, &MainWindow::handleError);
}

// ---------------------------------------------------------------------------------------------- //

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::toolBar() const -> QToolBar*
{
    return d->ui.toolBar;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::appendLog(const QString& msg)
{
    d->ui.logWidget->append(msg);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setConnected(ComponentFactory* factory)
{
    Q_ASSERT(d->componentFactory == nullptr);

    d->componentFactory = factory;
    d->ui.mainWidget->setConnected(factory);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setDisconnected()
{
    d->ui.mainWidget->setDisconnected();
    d->componentFactory = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::uploadFirmware(int uniqueId)
{
    Q_ASSERT(d->componentFactory != nullptr);

    if (!confirmUploadFirmware(uniqueId))
        return;

    try {
        ComponentPtr component = d->componentFactory->getComponent(uniqueId);

        UploadJob job(component.get());

        UploadDialog dialog(&job, this);
        connect(&dialog, &UploadDialog::message, d->ui.logWidget, &QTextEdit::append);

        dialog.exec();

        postUploadFirmware(uniqueId);
        refreshInfo();

        launchFirmware(uniqueId);
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::launchBootloader(int uniqueId)
{
    if (!confirmLaunchBootloader(uniqueId))
        return;

    try {
        ComponentPtr component = d->componentFactory->getComponent(uniqueId);
        component->launchBootloader();

        appendLog("Reset into bootloader successfully initiated.");

        postLaunchBootloader(uniqueId);
        refreshInfo();
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::launchFirmware(int uniqueId)
{
    if (!confirmLaunchFirmware(uniqueId))
        return;

    try {
        ComponentPtr component = d->componentFactory->getComponent(uniqueId);
        component->launchFirmware();

        appendLog("Reset into firmware successfully initiated.");

        postLaunchFirmware(uniqueId);
        refreshInfo();
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmUploadFirmware(int uniqueId) -> bool
{
    Q_UNUSED(uniqueId);
    return true;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postUploadFirmware(int uniqueId)
{
    Q_UNUSED(uniqueId);
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchBootloader(int uniqueId) -> bool
{
    Q_UNUSED(uniqueId);
    return true;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchBootloader(int uniqueId)
{
    Q_UNUSED(uniqueId);
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchFirmware(int uniqueId) -> bool
{
    Q_UNUSED(uniqueId);
    return true;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchFirmware(int uniqueId)
{
    Q_UNUSED(uniqueId);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleError(const QString& msg)
{
    appendLog(msg);
    QMessageBox::critical(this, "Error", msg);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::refreshInfo()
{
    d->ui.mainWidget->updateComponentInfo();
}

// ---------------------------------------------------------------------------------------------- //
