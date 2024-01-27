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

cmake_minimum_required(VERSION 3.14)
project(FirmwarePackager)

set(FIRMWARE_PACKAGER_USE_QT5 OFF CACHE BOOL "Build against Qt5 rather than Qt6.")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (FIRMWARE_PACKAGER_USE_QT5)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets)
else()
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
endif()

find_package(OpenSSL REQUIRED)
find_package(libzip REQUIRED)

set(FIRMWARE_PACKAGER_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        metadata.cpp
        metadata.h
        packager.cpp
        packager.h
        presetsmenu.cpp
        presetsmenu.h
)

set(FIRMWARE_PACKAGER_RESOURCES FirmwarePackager.qrc)

add_executable(FirmwarePackager WIN32 ${FIRMWARE_PACKAGER_SOURCES} ${FIRMWARE_PACKAGER_RESOURCES})
target_link_libraries(FirmwarePackager Qt::Widgets OpenSSL::Crypto zip)
