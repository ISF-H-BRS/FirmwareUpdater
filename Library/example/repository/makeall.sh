#!/bin/bash

rm -f NucleoF446RE-1.0-1.0.zip DummyMasterBoard-1.0-1.0.zip DummySlaveBoard-2.0-2.0.zip
sh makepkg.sh nucleo NucleoF446RE-1.0-1.0.zip ../keys/FirmwareUpdater.pem
sh makepkg.sh master DummyMasterBoard-1.0-1.0.zip ../keys/FirmwareUpdater.pem
sh makepkg.sh slave DummySlaveBoard-2.0-2.0.zip ../keys/FirmwareUpdater.pem
