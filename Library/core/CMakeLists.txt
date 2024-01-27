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

message("Building core library.")

find_package(OpenSSL REQUIRED)
find_package(libzip REQUIRED)

set(FIRMWARE_UPDATER_CORE_SOURCES
    base64.c
    firmwarearchive.cpp
    firmwaremanager.cpp
    uploadjob.cpp
)

set(FIRMWARE_UPDATER_CORE_HEADERS
    base64.h
    ../include/FirmwareUpdater/Core/component.h
    ../include/FirmwareUpdater/Core/componentfactory.h
    ../include/FirmwareUpdater/Core/firmwarearchive.h
    ../include/FirmwareUpdater/Core/firmwaremanager.h
    ../include/FirmwareUpdater/Core/namespace.h
    ../include/FirmwareUpdater/Core/uploadjob.h
)

set(FIRMWARE_UPDATER_CORE_LIBRARIES
    OpenSSL::Crypto
    zip
)

add_library(FirmwareUpdaterCore SHARED
    ${FIRMWARE_UPDATER_CORE_SOURCES}
    ${FIRMWARE_UPDATER_CORE_HEADERS}
)

target_link_libraries(FirmwareUpdaterCore ${FIRMWARE_UPDATER_CORE_LIBRARIES})

if (WIN32)
    set_target_properties(FirmwareUpdaterCore PROPERTIES PREFIX "")
endif()
