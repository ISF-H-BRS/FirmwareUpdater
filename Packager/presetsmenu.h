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

#pragma once

#include <QMenu>

class PresetsMenu : public QMenu
{
    Q_OBJECT

public:
    PresetsMenu(QWidget* parent = nullptr);

    void setCurrentPreset(const QString& name);
    auto currentPreset() const -> QString;

public slots:
    void reloadPresets();
    void setModified();

signals:
    void presetChanged(const QString& name);

    void saveRequested();
    void deleteRequested();

private slots:
    void updateSelection();

private:
    QList<QAction*> m_presets;

    QAction* m_modifiedAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_deleteAction = nullptr;
};
