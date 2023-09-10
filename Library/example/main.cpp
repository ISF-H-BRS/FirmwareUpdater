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
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIICCgKCAgEAox5HiWSXPB2kW9r7mLmjk7oE7U6D+FMWT1pR9QDnMgPEPMH0rzW3\n"
        "TbDnLFX8XRpMvjIvCgd0N2m+kvTM8/V+DRbGy4j2OgjvOfZWMUWqIvXajNmWz6mo\n"
        "R1bTaLj3BF3xP7qP1c0SjdGzI2W7P1sN2AVs1jWkbuB8xZapqL9Sq2f4Ua4snlDF\n"
        "f26fraBGiz0RV0oWiwym37Rm8qZEgYKO5+Di8q7pFJtomWygSaKDOEdVZi5jHZHT\n"
        "MEmA/UhoRH7M5jmHV5qG23Yvpt0vf90G0LaTw1UeGSvfN9qk6gcSg1O+7ZqMEZMt\n"
        "2wEljj2bSMOMLGmmBxbensLXrek/t9CRA3M6w9Tbi7iAty36vJKoV8vVM684Crhk\n"
        "LtpKC0htTqMLpuL0HO8ErMpvK4M+aoLR8EyBYsUEG2ZH3I+HXOPREaZnsrdrWomp\n"
        "6UQCgslSfeqwbR61Z9XKd+26L9zc+Vkvf/WyDVKDAyTet45jBKD8dBJi2QXuJLqr\n"
        "c0ooEtnRM3uwt58iKjvILk7kAuQGuTOW8toBoAxJfrlf0YfrPkjlbMRxgWWqovOo\n"
        "neRy5jTZtMdIfv9FaRARB1OuMMEpsA7kZlJSn824cgbyB+8hJMVKIgaPtaBiANXL\n"
        "PkwINBoVpjLWUOTO9jBi9S9+z//dYyLWuvEfK8WROLYqIHoY4hG8hdECAwEAAQ==\n"
        "-----END RSA PUBLIC KEY-----";
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
