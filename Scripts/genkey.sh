#!/bin/bash

name=$1

if [ -z "$name" ]; then
    echo "Usage: $0 <name>"; exit
fi

ssh-keygen -t rsa -b 4096 -m PEM -f "/tmp/$name"
openssl rsa -in "/tmp/$name" -outform pem > "$name.pem"
openssl rsa -in "/tmp/$name" -outform pem -pubout > "$name.pub.pem"
rm "/tmp/$name" "/tmp/$name.pub"
