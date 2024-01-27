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

#include "metadata.h"

#include <functional>
#include <stdexcept>

class Packager
{
public:
    struct Options
    {
        bool includeHardwareVersion = true;
        bool includeFirmwareVersion = true;
        bool saveMetadata = false;
    };

    using Error = std::runtime_error;

public:
    static auto getOutputFileName(const Metadata& metadata, const Options& options) -> QString;

    static void create(const QString& hexFile, const Metadata& metadata,
                       const QString& signingKey, std::function<QString()> password,
                       const QString& outputDirectory, const Options& options);

private:
    static void writeMetadata(const Metadata& metadata, const QString& filename);
};
