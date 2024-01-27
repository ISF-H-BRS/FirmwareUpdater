####################################################################################################
#                                                                                                  #
#   This file is part of the ISF Firmware Updater library.                                         #
#                                                                                                  #
#   Author:                                                                                        #
#   Marcel Hasler <mahasler@gmail.com>                                                             #
#                                                                                                  #
#   Copyright (c) 2020 - 2024                                                                      #
#   Bonn-Rhein-Sieg University of Applied Sciences                                                 #
#                                                                                                  #
#   This program is free software: you can redistribute it and/or modify it under the terms        #
#   of the GNU General Public License as published by the Free Software Foundation, either         #
#   version 3 of the License, or (at your option) any later version.                               #
#                                                                                                  #
#   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
#   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
#   See the GNU General Public License for more details.                                           #
#                                                                                                  #
#   You should have received a copy of the GNU General Public License along with this program.     #
#   If not, see <https:# www.gnu.org/licenses/>.                                                   #
#                                                                                                  #
####################################################################################################

message("Building example application.")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (FIRMWARE_UPDATER_USE_QT5)
    find_package(Qt5 5.15 COMPONENTS SerialPort Widgets REQUIRED)
else()
    find_package(Qt6 COMPONENTS SerialPort Widgets REQUIRED)
endif()

set(FIRMWARE_UPDATER_SOURCES
    aboutdialog.cpp
    componentfactory.cpp
    main.cpp
    mainwindow.cpp
)

set(FIRMWARE_UPDATER_HEADERS
    aboutdialog.h
    componentfactory.h
    mainwindow.h
)

set(FIRMWARE_UPDATER_FORMS
    aboutdialog.ui
)

set(FIRMWARE_UPDATER_RESOURCES FirmwareUpdater.qrc)

set(FIRMWARE_UPDATER_LIBRARIES
    FirmwareUpdaterUi
    Qt::Widgets
)

set(DUMMY_UPDATER_SOURCES
    ${FIRMWARE_UPDATER_SOURCES}
    mastercomponent.cpp
    slavecomponent.cpp
)

set(DUMMY_UPDATER_HEADERS
    ${FIRMWARE_UPDATER_HEADERS}
    dummydevice.h
    mastercomponent.h
    slavecomponent.h
)

set(NUCLEO_UPDATER_SOURCES
    ${FIRMWARE_UPDATER_SOURCES}
    nucleocomponent.cpp
    nucleodevice.cpp
    serialport.cpp
)

set(NUCLEO_UPDATER_HEADERS
    ${FIRMWARE_UPDATER_HEADERS}
    nucleocomponent.h
    nucleodevice.h
    serialport.h
)

set(NUCLEO_UPDATER_LIBRARIES
    ${FIRMWARE_UPDATER_LIBRARIES}
    Qt::SerialPort
)

add_executable(DummyUpdater WIN32
    ${DUMMY_UPDATER_SOURCES}
    ${DUMMY_UPDATER_HEADERS}
    ${FIRMWARE_UPDATER_FORMS}
    ${FIRMWARE_UPDATER_RESOURCES}
)

target_compile_definitions(DummyUpdater PRIVATE FIRMWAREUPDATER_BUILD_DUMMY)
target_link_libraries(DummyUpdater ${FIRMWARE_UPDATER_LIBRARIES})

add_executable(NucleoUpdater WIN32
    ${NUCLEO_UPDATER_SOURCES}
    ${NUCLEO_UPDATER_HEADERS}
    ${FIRMWARE_UPDATER_FORMS}
    ${FIRMWARE_UPDATER_RESOURCES}
)

target_link_libraries(NucleoUpdater ${NUCLEO_UPDATER_LIBRARIES})
