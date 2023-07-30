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

#include <FirmwareUpdater/Core/uploadjob.h>

#include <QDialog>

#include <memory>

namespace Ui {
    class UploadDialog;
}

FIRMWAREUPDATER_BEGIN_NAMESPACE()

class UploadDialog : public QDialog
{
    Q_OBJECT

public:
    UploadDialog(UploadJob* job, QWidget* parent = nullptr);
    ~UploadDialog() override;

    int exec() override;

signals:
    void message(const QString& msg);

private:
    std::unique_ptr<Ui::UploadDialog> m_ui;
    UploadJob* m_job;
};

FIRMWAREUPDATER_END_NAMESPACE()
