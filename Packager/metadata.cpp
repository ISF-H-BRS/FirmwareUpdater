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

#include "metadata.h"

#include <QFile>

// ---------------------------------------------------------------------------------------------- //

auto Metadata::fromFile(const QString& filename) -> Metadata
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly))
        throw Error("Unable to open metadata file for reading.");

    Metadata metadata;

    while (!file.atEnd())
    {
        QString line = file.readLine();

        if (line.isEmpty())
            continue;

        QStringList tokens = line.split('=');

        if (tokens.size() != 2)
            continue;

        const QString& key = tokens.at(0).trimmed();
        const QString& value = tokens.at(1).trimmed();

        if (key == "BoardName")
            metadata.boardName = value;
        else if (key == "HardwareVersion")
            metadata.hardwareVersion = value;
        else if (key == "FirmwareVersion")
            metadata.firmwareVersion = value;
        else if (key == "ReleaseDate")
            metadata.releaseDate = value;
        else if (key == "Packager")
            metadata.packager = value;
    }

    return metadata;
}

// ---------------------------------------------------------------------------------------------- //
