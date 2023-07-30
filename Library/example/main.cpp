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

#include "mainwindow.h"

#include <FirmwareUpdater/core.h>

#include <QApplication>

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr const char* PublicKey =
            "-----BEGIN PUBLIC KEY-----\n"
            "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA1vFeef+TObySEW27LCT1"
            "uK2fS3AykwcZUMVAB3c+KB3nAz1Oj5NAxrh//vMKiH9CZD8o8qjChmRRYV7FGEnZ"
            "HqpwntCB1kl7iInBiDMXxPivLYPmeIfW0FgO6fD9pHVOP67dFko8EwKlpG5YIJat"
            "DhkSNkdgOHe+5BsxaNtjPXnGoriTctDLTMh6SxxBgHu9eK8PR9fyPmI5NPr+pgOW"
            "nkr5EIvrLGUiYU1gqSEmznvQkXIFzCikLxQOh6IJY7M8oHVpGZOxpcYBIcDAqI+I"
            "rnu3v3YrtBlxlAjRGmRkSqv+hbFLw0sveVBjxYnP7dsE2RmM5f7G6Q2bgePM7oA1"
            "BFdN/G6xak1G7sf1Km6e0V8HksXmGwuojLX71fVEuuD/o5uDElkl6aKSxX35Pipk"
            "wSn3GIzwp0JzproVGz9VbgMQubFJXaABCeldHDRtiCNYhnOhjK2GsPO3DRIPXee2"
            "vj+JLdvQS8QRTCR+dQlkl5fHBP+o8RKpjS8jiyCet3BcEEuUGM9lk7c+3UmQ1L1a"
            "I8PNlEnUZT2qkK4JwUh/wveik8lnZng9SG+XlFYeGE8tpJiCOKFCU6tUHIQdmNOP"
            "Dux1DoFL5GwA7dofzEllNiTSZvj+UFRQaXiWACd7UQ/0Z40s/dMgEuvjE4YXgp5e"
            "rrzDnx0kXNXJCd4RxWiRNPMCAwEAAQ==\n"
            "-----END PUBLIC KEY-----";
}

// ---------------------------------------------------------------------------------------------- //

auto main(int argc, char* argv[]) -> int
{
    FirmwareUpdater::FirmwareArchive::setPublicKey(PublicKey);

    QCoreApplication::setOrganizationName("Bonn-Rhein-Sieg University of Applied Sciences");
    QCoreApplication::setApplicationName("ISF Firmware Updater");
    QCoreApplication::setApplicationVersion("1.0");

    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return QApplication::exec();
}

// ---------------------------------------------------------------------------------------------- //
