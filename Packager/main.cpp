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

#include <QApplication>

// ---------------------------------------------------------------------------------------------- //

auto main(int argc, char* argv[]) -> int
{
    QApplication::setOrganizationName("Bonn-Rhein-Sieg University of Applied Sciences");
    QApplication::setApplicationName("ISF Firmware Packager");
    QApplication::setApplicationVersion("1.0");

    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return QApplication::exec();
}

// ---------------------------------------------------------------------------------------------- //
