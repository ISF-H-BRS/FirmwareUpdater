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

#include "aboutdialog.h"
#include "mainwindow.h"

#ifdef FIRMWAREUPDATER_BUILD_DUMMY
#include "dummydevice.h"
#else
#include "nucleodevice.h"
#endif

#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>

#ifndef FIRMWAREUPDATER_BUILD_DUMMY
#include <QSerialPortInfo>
#endif

#include <thread>

// ---------------------------------------------------------------------------------------------- //

MainWindow::MainWindow(QWidget* parent)
    : FirmwareUpdater::MainWindow(parent)
{
    m_actionConnect = toolBar()->addAction(QIcon(":/res/connect.svg"), "Connect");
    m_actionDisconnect = toolBar()->addAction(QIcon(":/res/disconnect.svg"), "Disconnect");
    m_actionAbout = toolBar()->addAction(QIcon(":/res/about.svg"), "About");

    m_actionConnect->setEnabled(true);
    m_actionDisconnect->setEnabled(false);

    connect(m_actionConnect, &QAction::triggered, this, &MainWindow::openDevice);
    connect(m_actionDisconnect, &QAction::triggered, this, &MainWindow::closeDevice);
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    appendLog("Client started.");
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::openDevice()
{
    Q_ASSERT(m_componentFactory == nullptr);

#ifndef FIRMWAREUPDATER_BUILD_DUMMY
    const QString port = getSerialPort();

    if (port.isEmpty())
        return;
#endif

    try {
#ifdef FIRMWAREUPDATER_BUILD_DUMMY
        auto device = std::make_unique<Device>();
#else
        auto device = std::make_unique<Device>(port.toUtf8());
#endif

        m_componentFactory = std::make_unique<ComponentFactory>(std::move(device));
        appendLog("Connected to device.");

        m_actionConnect->setEnabled(false);
        m_actionDisconnect->setEnabled(true);

        setConnected(m_componentFactory.get());
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::closeDevice()
{
    if (!m_componentFactory)
        return;

    setDisconnected();

    m_actionConnect->setEnabled(true);
    m_actionDisconnect->setEnabled(false);

    m_componentFactory = nullptr;
    appendLog("Connection to device closed.");
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showAboutDialog()
{
    AboutDialog dialog(this);
    dialog.exec();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleError(const QString& msg)
{
    FirmwareUpdater::MainWindow::handleError(msg);
    closeDevice();
}

// ---------------------------------------------------------------------------------------------- //

#ifdef FIRMWAREUPDATER_BUILD_DUMMY

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchBootloader(int uniqueId) -> bool
{
    if (uniqueId == ComponentFactory::MasterUniqueId)
        return confirmLaunchMasterBootloader();

    return confirmLaunchSlaveBootloader();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchBootloader(int uniqueId)
{
    if (uniqueId == ComponentFactory::MasterUniqueId)
        closeDevice();
    else
        wait(2s, "Waiting for bootloader...");
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchFirmware(int uniqueId) -> bool
{
    if (uniqueId == ComponentFactory::MasterUniqueId)
        return confirmLaunchMasterFirmware();

    return confirmLaunchSlaveFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchFirmware(int uniqueId)
{
    if (uniqueId == ComponentFactory::MasterUniqueId)
        closeDevice();
    else
        wait(2s, "Waiting for firmware...");
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchMasterBootloader() -> bool
{
    const QString msg = "The device will now reset to bootloader mode. "
                        "Please wait for the hardware to reinitialize, then reconnect.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchSlaveBootloader() -> bool
{
    const QString msg = "The device will now reset to bootloader mode.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchMasterFirmware() -> bool
{
    const QString msg = "The device will now reset to firmware mode. "
                        "Please wait for the hardware to reinitialize, then reconnect.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchSlaveFirmware() -> bool
{
    const QString msg = "The device will now reset to firmware mode.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return (button == QMessageBox::Ok);
}

// ---------------------------------------------------------------------------------------------- //

#else // !FIRMWAREUPDATER_BUILD_DUMMY

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchBootloader(int uniqueId) -> bool
{
    Q_ASSERT(uniqueId == ComponentFactory::NucleoUniqueId);

    const QString msg = "The device will now reset to bootloader mode.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchBootloader(int uniqueId)
{
    Q_ASSERT(uniqueId == ComponentFactory::NucleoUniqueId);
    wait(1s, "Waiting for bootloader...");

}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchFirmware(int uniqueId) -> bool
{
    Q_ASSERT(uniqueId == ComponentFactory::NucleoUniqueId);

    const QString msg = "The device will now reset to firmware mode.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchFirmware(int uniqueId)
{
    Q_ASSERT(uniqueId == ComponentFactory::NucleoUniqueId);
    wait(1s, "Waiting for firmware...");
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::getSerialPort() -> QString
{
    const auto ports = QSerialPortInfo::availablePorts();

    if (ports.isEmpty())
    {
        QMessageBox::warning(this, "Note", "No usable serial ports found.");
        return "";
    }

    QStringList items;

    for (const QSerialPortInfo& info : ports)
        items << info.portName();

    bool ok = false;
    QString port = QInputDialog::getItem(this, "Select Serial Port", "Serial port:", items,
                                         0, false, &ok);
    if (!ok)
        return "";

    return port;
}

// ---------------------------------------------------------------------------------------------- //

#endif // FIRMWAREUPDATER_BUILD_DUMMY

// ---------------------------------------------------------------------------------------------- //

void MainWindow::wait(std::chrono::milliseconds ms, QString label)
{
    static constexpr auto TickDuration = 100ms;
    const auto ticks = static_cast<int>(ms / TickDuration);

    if (label.isEmpty())
        label = "Please wait...";

    QProgressDialog dialog(label, QString(), 0, ticks, this);
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    dialog.setWindowTitle("Please Wait");
    dialog.show();

    for (int i = 0; i < ticks; ++i)
    {
        std::this_thread::sleep_for(TickDuration);
        dialog.setValue(i + 1);
        QApplication::processEvents();
    }
}

// ---------------------------------------------------------------------------------------------- //
