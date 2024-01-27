// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater software.                                       //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2024                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This program is free software: you can redistribute it and/or modify it under the terms       //
//  of the GNU General Public License as published by the Free Software Foundation, either        //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU General Public License for more details.                                          //
//                                                                                                //
//  You should have received a copy of the GNU General Public License along with this program.    //
//  If not, see <https://www.gnu.org/licenses/>.                                                  //
//                                                                                                //
// ============================================================================================== //

#include "presetsmenu.h"

#include <QMessageBox>
#include <QSettings>

// ---------------------------------------------------------------------------------------------- //

PresetsMenu::PresetsMenu(QWidget* parent)
    : QMenu(parent)
{
    reloadPresets();
}

// ---------------------------------------------------------------------------------------------- //

void PresetsMenu::setCurrentPreset(const QString& name)
{
    setDefaultAction(nullptr);

    for (auto preset : m_presets)
    {
        if (preset->text() == name)
        {
            setDefaultAction(preset);
            preset->setIcon(QIcon(":/res/bullet.svg"));
        }
        else
            preset->setIcon(QIcon());
    }

    const bool selected = defaultAction() != nullptr;

    m_modifiedAction->setVisible(!selected);
    m_saveAction->setEnabled(!selected);
    m_deleteAction->setEnabled(selected);
}

// ---------------------------------------------------------------------------------------------- //

auto PresetsMenu::currentPreset() const -> QString
{
    auto preset = defaultAction();
    return preset ? preset->text() : "";
}

// ---------------------------------------------------------------------------------------------- //

void PresetsMenu::reloadPresets()
{
    clear();
    m_presets.clear();

    QSettings settings;
    auto currentPreset = settings.value("CurrentPreset").toString();

    QFont font;
    font.setItalic(true);

    m_modifiedAction = addAction("New preset");
    m_modifiedAction->setFont(font);
    m_modifiedAction->setEnabled(false);
    m_modifiedAction->setVisible(false);

    settings.beginGroup("Presets");

    for (const auto& name : settings.childGroups())
    {
        auto preset = new QAction(name, this);
        m_presets.append(preset);

        connect(preset, &QAction::triggered, this, &PresetsMenu::updateSelection);
    }

    std::sort(m_presets.begin(), m_presets.end(),
              [](QAction* p1, QAction* p2){ return p1->text() < p2->text(); });

    addActions(m_presets);
    addSeparator();

    m_saveAction = addAction("Save Preset", this, &PresetsMenu::saveRequested);
    m_saveAction->setIcon(QIcon(":/res/save.svg"));
    m_saveAction->setEnabled(false);

    m_deleteAction = addAction("Delete Preset", this, &PresetsMenu::deleteRequested);
    m_deleteAction->setIcon(QIcon(":/res/delete.svg"));
    m_deleteAction->setEnabled(defaultAction() != nullptr);

    setCurrentPreset(currentPreset);
}

// ---------------------------------------------------------------------------------------------- //

void PresetsMenu::setModified()
{
    setCurrentPreset("");
}

// ---------------------------------------------------------------------------------------------- //

void PresetsMenu::updateSelection()
{
    auto preset = qobject_cast<QAction*>(sender());
    Q_ASSERT(preset != nullptr);

    setCurrentPreset(preset->text());
    emit presetChanged(preset->text());
}

// ---------------------------------------------------------------------------------------------- //
