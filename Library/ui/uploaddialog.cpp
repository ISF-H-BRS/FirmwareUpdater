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

#include "uploaddialog.h"
#include "ui_uploaddialog.h"

#include <QMessageBox>

// ---------------------------------------------------------------------------------------------- //

using namespace FirmwareUpdater;

// ---------------------------------------------------------------------------------------------- //

UploadDialog::UploadDialog(UploadJob* job, QWidget* parent)
    : QDialog(parent),
      m_ui(std::make_unique<Ui::UploadDialog>()),
      m_job(job)
{
    Q_ASSERT(job != nullptr);

    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setWindowModality(Qt::ApplicationModal);

    m_ui->setupUi(this);
}

// ---------------------------------------------------------------------------------------------- //

UploadDialog::~UploadDialog() = default;

// ---------------------------------------------------------------------------------------------- //

int UploadDialog::exec()
{
    const auto progress = [&](int erase, int upload, int total)
    {
        m_ui->eraseProgress->setValue(erase);
        m_ui->uploadProgress->setValue(upload);
        m_ui->totalProgress->setValue(total);

        QApplication::processEvents();
    };

    const auto msg = [&](const std::string& msg)
    {
        emit message(msg.c_str());
        QApplication::processEvents();
    };

    show();
    m_job->run(progress, msg);

    QMessageBox::information(this, "Upload Complete", "Firmware upload complete.");

    return QDialog::Accepted;
}

// ---------------------------------------------------------------------------------------------- //
