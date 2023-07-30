# ISF Firmware Updater

## Introduction

ISF Firmware Updater is a framework intended to facilitate the development of custom firmware-updating applications.

It consists of two parts: A bootloader framework for STM32 microcontrollers and a software framework for desktop applications.

The software framework consists of a core library that implements the actual firmware and device handling and a Qt-based UI library which provides a complete user interface that can be extended as needed. The core library can be used to implement command-line tools or to incorporate firmware-update functionality into existing applications.

The framework uses cryptographically signed firmware packages with embedded metadata to ensure that only firmware actually intended for a particular device will be uploaded.

The bootloader is designed to always run on power-on-reset. On startup it will read a dedicated partition in SRAM containing bootflags indicating a reboot-to-bootloader from running firmware. Unless set (i.e. after power-on) it will compute a CRC-32 checksum over the entire firmware partition and compare it to a checksum saved in flash memory during programming. Only in case of a match the firmware is then booted, otherwise the device will remain in the bootloader waiting for valid firmware data to be uploaded.

The bootloader framework doesn't dictate any particular communication protocol. This allows the same protocol to be used for both the firmware and the bootloader. Also the bootloader can run over any communication interface supported by the device, including such not supported by the bootloader contained in the microcontroller's ROM as provided by ST.

This ability has e.g. been used in practice to indirectly upload the firmware for sensor nodes communicating over RS-485 with a central hub, itself communicating over either virtual serial port or TCP/IP with a host system.

## Examples

The Bootloader directory contains a working example for a Nucleo-F446RE.

The Library directory contains an example that will compile two executables, one for use with the Nucleo board and the other emulating a dummy device that can be used to test the framework without existing hardware.

The Scripts directory contains two Bash scripts to help create signed firmware packages. In order to run them, [openssl](https://www.openssl.org/) and [zip](https://infozip.sourceforge.net/) are required.

At some point a package-generator application might be added to the project, but for now, have a look at the 'repository' directory of the example application for a working example.

## License

In order to avoid any legal complications with proprieteray firmware projects the bootloader framework is released under the permissive 2-Clause BSD License. See [COPYING](Bootloader/COPYING) for details.

The software framework is released under the GNU Lesser General Public License (LGPL). See [COPYING](Library/COPYING) along with [COPYING.LESSER](Library/COPYING.LESSER) for details.
