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

#include <FirmwareUpdater/Core/firmwaremanager.h>

#include "mainwidget.h"
#include "ui_mainwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::MainWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->componentList, &QListWidget::currentItemChanged,
            this, &MainWidget::onCurrentItemChanged);

    connect(m_ui->componentWidget, &ComponentWidget::uploadFirmwareClicked,
            this, &MainWidget::onUploadFirmwareClicked);

    connect(m_ui->componentWidget, &ComponentWidget::launchFirmwareClicked,
            this, &MainWidget::onLaunchFirmwareClicked);

    connect(m_ui->componentWidget, &ComponentWidget::launchBootloaderClicked,
            this, &MainWidget::onLaunchBootloaderClicked);
}

// ---------------------------------------------------------------------------------------------- //

MainWidget::~MainWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void MainWidget::setConnected(ComponentFactory* factory)
{
    Q_ASSERT(factory != nullptr);

    m_componentFactory = factory;
    m_ui->mainStack->setCurrentWidget(m_ui->componentPage);

    const ComponentList components = factory->getComponentList();

    for (const auto& component: components)
    {
        auto item = new QListWidgetItem();
        item->setData(Qt::UserRole, component.uniqueId);
        item->setText(component.displayName.c_str());
        m_ui->componentList->addItem(item);
    }

    if (m_ui->componentList->count() > 0)
    {
        m_ui->componentWidget->setEnabled(true);
        m_ui->componentList->setCurrentRow(0);
        m_ui->componentList->setMinimumWidth(m_ui->componentList->sizeHintForColumn(0));

        updateComponentInfo();
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::setDisconnected()
{
    m_componentFactory = nullptr;
    m_ui->componentList->clear();
    m_ui->mainStack->setCurrentWidget(m_ui->defaultPage);
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::updateComponentInfo()
{
    if (!m_componentFactory)
        return;

    try {
        updateComponentWidget(getCurrentComponentId());
    }
    catch (const std::exception& e) {
        emit error(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    Q_UNUSED(previous)

    if (current)
        updateComponentInfo();
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::onUploadFirmwareClicked()
{
    emit uploadFirmwareClicked(getCurrentComponentId());
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::onLaunchFirmwareClicked()
{
    emit launchFirmwareClicked(getCurrentComponentId());
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::onLaunchBootloaderClicked()
{
    emit launchBootloaderClicked(getCurrentComponentId());
}

// ---------------------------------------------------------------------------------------------- //

void MainWidget::updateComponentWidget(int uniqueId)
{
    Q_ASSERT(m_componentFactory != nullptr);

    const auto getFirmwareVersion = [](const auto& info) -> std::string {
        return FirmwareManager::getAvailableVersion(info.boardName, info.hardwareVersion);
    };

    ComponentPtr component = m_componentFactory->getComponent(uniqueId);

    const Component::BootMode mode = component->getBootMode();
    m_ui->componentWidget->setBootMode(mode);

    if (mode == Component::BootMode::Bootloader)
    {
        const Component::BootloaderInfo info = component->getBootloaderInfo();
        m_ui->componentWidget->setBootloaderInfo(info);
        m_ui->componentWidget->setAvailableFirmwareVersion(getFirmwareVersion(info));
    }
    else
    {
        const Component::FirmwareInfo info = component->getFirmwareInfo();
        m_ui->componentWidget->setFirmwareInfo(info);
        m_ui->componentWidget->setAvailableFirmwareVersion(getFirmwareVersion(info));
    }
}

// ---------------------------------------------------------------------------------------------- //

auto MainWidget::getCurrentComponentId() const -> int
{
    const QListWidgetItem* current = m_ui->componentList->currentItem();
    Q_ASSERT(current != nullptr);

    return current->data(Qt::UserRole).value<int>();
}

// ---------------------------------------------------------------------------------------------- //
