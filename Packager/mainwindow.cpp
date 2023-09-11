// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF Firmware Updater software.                                       //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2023                                                                     //
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

#include "mainwindow.h"
#include "packager.h"

#include "ui_mainwindow.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFontMetrics>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>

// ---------------------------------------------------------------------------------------------- //

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);
    m_ui->hexFile->setMinimumWidth(40 * QFontMetrics(QApplication::font()).horizontalAdvance('x'));

    restoreSettings();

    updateContinueToMetadataButton();
    updateContinueToOutputButton();
    updateCreatePackageButton();

    resize(minimumSizeHint());

    connect(m_ui->selectHexFile, &QToolButton::clicked,
            this, &MainWindow::selectHexFile);
    connect(m_ui->selectMetadataFile, &QToolButton::clicked,
            this, &MainWindow::selectMetadataFile);
    connect(m_ui->selectSigningKey, &QToolButton::clicked,
            this, &MainWindow::selectSigningKey);
    connect(m_ui->selectOutputDirectory, &QToolButton::clicked,
            this, &MainWindow::selectOutputDirectory);

    connect(m_ui->updateReleaseDate, &QToolButton::clicked, this, &MainWindow::updateReleaseDate);

    connect(m_ui->hexFile, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToMetadataButton);
    connect(m_ui->metadataFile, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToMetadataButton);
    connect(m_ui->signingKey, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToMetadataButton);

    connect(m_ui->boardName, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToOutputButton);
    connect(m_ui->hardwareVersion, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToOutputButton);
    connect(m_ui->firmwareVersion, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToOutputButton);
    connect(m_ui->releaseDate, &QDateEdit::dateChanged,
            this, &MainWindow::updateContinueToOutputButton);
    connect(m_ui->packager, &QLineEdit::textChanged,
            this, &MainWindow::updateContinueToOutputButton);

    connect(m_ui->outputDirectory, &QLineEdit::textChanged,
            this, &MainWindow::updateCreatePackageButton);

    connect(m_ui->continueToMetadata, &QPushButton::clicked, this, &MainWindow::continueToMetadata);
    connect(m_ui->returnToInput, &QPushButton::clicked, this, &MainWindow::returnToInput);

    connect(m_ui->continueToOutput, &QPushButton::clicked, this, &MainWindow::continueToOutput);
    connect(m_ui->returnToMetadata, &QPushButton::clicked, this, &MainWindow::returnToMetadata);

    connect(m_ui->createPackage, &QPushButton::clicked, this, &MainWindow::createPackage);
}

// ---------------------------------------------------------------------------------------------- //

MainWindow::~MainWindow()
{
    saveSettings();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::selectHexFile()
{
    const QString filename = QFileDialog::getOpenFileName(this, "Select HEX File",
                                                          m_ui->hexFile->text(),
                                                          "HEX Files (*.hex)");
    if (filename.isEmpty())
        return;

    m_ui->hexFile->setText(filename);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::selectMetadataFile()
{
    const QString filename = QFileDialog::getOpenFileName(this, "Select METADATA File",
                                                          m_ui->metadataFile->text(),
                                                          "METADATA");
    if (filename.isEmpty())
        return;

    m_ui->metadataFile->setText(filename);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::selectSigningKey()
{
    const QString filename = QFileDialog::getOpenFileName(this, "Select Signing Key",
                                                          m_ui->signingKey->text(),
                                                          "Signing Keys (*.pem *.key)");
    if (filename.isEmpty())
        return;

    m_ui->signingKey->setText(filename);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::selectOutputDirectory()
{
    const QString directory = QFileDialog::getExistingDirectory(this, "Select Output Directory",
                                                                m_ui->outputDirectory->text());
    if (directory.isEmpty())
        return;

    m_ui->outputDirectory->setText(directory);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateReleaseDate()
{
    m_ui->releaseDate->setDate(QDate::currentDate());
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::continueToMetadata()
{
    try {
        if (!m_ui->metadataFile->text().isEmpty())
            parseMetadataFile();

        m_ui->stack->setCurrentWidget(m_ui->metadataPage);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::returnToInput()
{
    m_ui->stack->setCurrentWidget(m_ui->inputPage);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::continueToOutput()
{
    m_ui->stack->setCurrentWidget(m_ui->outputPage);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::returnToMetadata()
{
    m_ui->stack->setCurrentWidget(m_ui->metadataPage);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::parseMetadataFile()
{
    const QString metadataFile = m_ui->metadataFile->text();
    Q_ASSERT(!metadataFile.isEmpty() && QFile::exists(metadataFile));

    const auto metadata = Metadata::fromFile(metadataFile);

    if (!metadata.boardName.isEmpty())
        m_ui->boardName->setText(metadata.boardName);

    if (!metadata.hardwareVersion.isEmpty())
        m_ui->hardwareVersion->setText(metadata.hardwareVersion);

    if (!metadata.firmwareVersion.isEmpty())
        m_ui->firmwareVersion->setText(metadata.firmwareVersion);

    if (!metadata.releaseDate.isEmpty())
        m_ui->releaseDate->setDate(QDate::fromString(metadata.releaseDate, "yyyyMMdd"));

    if (!metadata.packager.isEmpty())
        m_ui->packager->setText(metadata.packager);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::createPackage()
{
    const Metadata metadata {
        .boardName = m_ui->boardName->text(),
        .hardwareVersion = m_ui->hardwareVersion->text(),
        .firmwareVersion = m_ui->firmwareVersion->text(),
        .releaseDate = m_ui->releaseDate->date().toString("yyyyMMdd"),
        .packager = m_ui->packager->text()
    };

    const Packager::Options options = {
        .includeHardwareVersion = m_ui->includeHardwareVersion->isChecked(),
        .includeFirmwareVersion = m_ui->includeFirmwareVersion->isChecked(),
        .saveMetadata = m_ui->saveMetadata->isChecked()
    };

    bool passwordProvided = true;

    const auto getPassword = [this, &passwordProvided]
    {
        return QInputDialog::getText(this, "Enter Passphrase",
                                     "Enter the passphrase for the signing key:",
                                     QLineEdit::Password, QString(), &passwordProvided);
    };

    const QString fileName = Packager::getOutputFileName(metadata, options);
    const QString filePath = m_ui->outputDirectory->text() + "/" + fileName;

    if (QFile::exists(filePath))
    {
        QMessageBox::StandardButton button =
            QMessageBox::warning(this, "File Exists",
                                 "The firmware package " + fileName + " already exists "
                                 "in the selected output directory. If you choose to continue "
                                 "it will be replaced.", QMessageBox::Ok | QMessageBox::Cancel);

        if (button == QMessageBox::Cancel)
            return;
    }

    if (options.saveMetadata && QFile::exists(m_ui->outputDirectory->text() + "/METADATA"))
    {
        QMessageBox::StandardButton button =
            QMessageBox::warning(this, "File Exists",
                                 "The file METADATA already exists in the selected output "
                                 "directory. If you choose to continue it will be replaced.",
                                 QMessageBox::Ok | QMessageBox::Cancel);

        if (button == QMessageBox::Cancel)
            return;
    }

    try {
        Packager::create(m_ui->hexFile->text(), metadata,
                         m_ui->signingKey->text(), getPassword,
                         m_ui->outputDirectory->text(), options);

        QMessageBox::information(this, "Packaging Complete", "The package has been created.");
    }
    catch (const std::exception& e)
    {
        if (passwordProvided) // If the password entry was canceled we assume that to be the cause
            QMessageBox::critical(this, "Error", e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateContinueToMetadataButton()
{
    const QString hexFile = m_ui->hexFile->text();
    const QString metadataFile = m_ui->metadataFile->text();
    const QString signingKey = m_ui->signingKey->text();

    const bool enable = (!hexFile.isEmpty()      && QFile::exists(hexFile))      &&
                        ( metadataFile.isEmpty() || QFile::exists(metadataFile)) &&
                        (!signingKey.isEmpty()   && QFile::exists(signingKey));

    m_ui->continueToMetadata->setEnabled(enable);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateContinueToOutputButton()
{
    const QString boardName = m_ui->boardName->text();
    const QString hardwareVersion = m_ui->hardwareVersion->text();
    const QString firmwareVersion = m_ui->firmwareVersion->text();
    const QString releaseDate = m_ui->releaseDate->text();
    const QString packager = m_ui->packager->text();

    const bool enable = !(boardName.isEmpty()       ||
                          hardwareVersion.isEmpty() ||
                          firmwareVersion.isEmpty() ||
                          releaseDate.isEmpty()     ||
                          packager.isEmpty());

    m_ui->continueToOutput->setEnabled(enable);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateCreatePackageButton()
{
    const QString outputDirectory = m_ui->outputDirectory->text();

    const bool enable = !outputDirectory.isEmpty() && QDir(outputDirectory).exists();

    m_ui->createPackage->setEnabled(enable);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("HexFile", m_ui->hexFile->text());
    settings.setValue("MetadataFile", m_ui->metadataFile->text());
    settings.setValue("SigningKey", m_ui->signingKey->text());
    settings.setValue("BoardName", m_ui->boardName->text());
    settings.setValue("HardwareVersion", m_ui->hardwareVersion->text());
    settings.setValue("FirmwareVersion", m_ui->firmwareVersion->text());
    settings.setValue("ReleaseDate", m_ui->releaseDate->date());
    settings.setValue("Packager", m_ui->packager->text());
    settings.setValue("OutputDirectory", m_ui->outputDirectory->text());
    settings.setValue("IncludeHardwareVersion", m_ui->includeHardwareVersion->isChecked());
    settings.setValue("IncludeFirmwareVersion", m_ui->includeFirmwareVersion->isChecked());
    settings.setValue("SaveMetadata", m_ui->saveMetadata->isChecked());
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::restoreSettings()
{
    QSettings settings;
    QString text;
    QDate date;
    bool check = false;

    text = settings.value("HexFile", m_ui->hexFile->text()).toString();
    m_ui->hexFile->setText(text);

    text = settings.value("MetadataFile", m_ui->metadataFile->text()).toString();
    m_ui->metadataFile->setText(text);

    text = settings.value("SigningKey", m_ui->signingKey->text()).toString();
    m_ui->signingKey->setText(text);

    text = settings.value("BoardName", m_ui->boardName->text()).toString();
    m_ui->boardName->setText(text);

    text = settings.value("HardwareVersion", m_ui->hardwareVersion->text()).toString();
    m_ui->hardwareVersion->setText(text);

    text = settings.value("FirmwareVersion", m_ui->firmwareVersion->text()).toString();
    m_ui->firmwareVersion->setText(text);

    date = settings.value("ReleaseDate", m_ui->releaseDate->date()).toDate();
    m_ui->releaseDate->setDate(date);

    text = settings.value("Packager", m_ui->packager->text()).toString();
    m_ui->packager->setText(text);

    text = settings.value("OutputDirectory", m_ui->outputDirectory->text()).toString();
    m_ui->outputDirectory->setText(text);

    check = settings.value("IncludeHardwareVersion",
                           m_ui->includeHardwareVersion->isChecked()).toBool();
    m_ui->includeHardwareVersion->setChecked(check);

    check = settings.value("IncludeFirmwareVersion",
                           m_ui->includeFirmwareVersion->isChecked()).toBool();
    m_ui->includeFirmwareVersion->setChecked(check);

    check = settings.value("SaveMetadata", m_ui->saveMetadata->isChecked()).toBool();
    m_ui->saveMetadata->setChecked(check);
}

// ---------------------------------------------------------------------------------------------- //
