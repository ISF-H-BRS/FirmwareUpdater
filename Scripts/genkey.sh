#!/bin/bash

name=$1

if [ -z "$name" ]; then
    echo "Usage: $0 <name>"; exit
fi

ssh-keygen -t rsa -b 4096 -m pem -f "$name"
mv "$name" "$name.pem"
mv "$name.pub" "$name.pub.ssh"
ssh-keygen -f "$name.pub.ssh" -e -m pem > "$name.pub.pem"
rm "$name.pub.ssh"
