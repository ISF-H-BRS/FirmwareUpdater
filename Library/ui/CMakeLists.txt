####################################################################################################
#                                                                                                  #
#   This file is part of the ISF Firmware Updater library.                                         #
#                                                                                                  #
#   Author:                                                                                        #
#   Marcel Hasler <mahasler@gmail.com>                                                             #
#                                                                                                  #
#   Copyright (c) 2020 - 2023                                                                      #
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

message("Building UI library.")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if (FIRMWARE_UPDATER_USE_QT5)
    find_package(Qt5 5.15 COMPONENTS Widgets REQUIRED)
else()
    find_package(Qt6 COMPONENTS Widgets REQUIRED)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(FIRMWARE_UPDATER_UI_SOURCES
    bootloaderwidget.cpp
    componentwidget.cpp
    firmwarewidget.cpp
    mainwidget.cpp
    mainwindow.cpp
    uploaddialog.cpp
)

set(FIRMWARE_UPDATER_UI_HEADERS
    bootloaderwidget.h
    componentwidget.h
    firmwarewidget.h
    mainwidget.h
    uploaddialog.h
    ../include/FirmwareUpdater/Ui/mainwindow.h
)

set(FIRMWARE_UPDATER_UI_FORMS
    bootloaderwidget.ui
    componentwidget.ui
    firmwarewidget.ui
    mainwidget.ui
    mainwindow.ui
    uploaddialog.ui
)

set(FIRMWARE_UPDATER_UI_LIBRARIES
    FirmwareUpdaterCore
    Qt::Widgets
)

add_library(FirmwareUpdaterUi SHARED
    ${FIRMWARE_UPDATER_UI_SOURCES}
    ${FIRMWARE_UPDATER_UI_HEADERS}
    ${FIRMWARE_UPDATER_UI_FORMS}
)

target_link_libraries(FirmwareUpdaterUi ${FIRMWARE_UPDATER_UI_LIBRARIES})
