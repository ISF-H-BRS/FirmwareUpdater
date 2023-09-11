#!/bin/bash

name=$1

if [ -z "$name" ]; then
    echo "Usage: $0 <name>"; exit
fi

ssh-keygen -m pem -f "$name" -t rsa -b 4096
ssh-keygen -m pem -f "$name.pub" -e > "$name.pub.pem"
mv "$name" "$name.pem"
rm "$name.pub"
