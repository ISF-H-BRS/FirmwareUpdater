#!/bin/bash

ZIP="zip -j" # Don't include directories

hexfile="Data.hex"
metadata="METADATA"

tmpdir="/tmp/makepkg"
zipfile="$tmpdir/Data.zip"
sigfile="$zipfile.sig"
tmpfile="$sigfile.tmp"

directory=$1
outfile=$2
key=$3

if [[ -z "$directory" || -z "$outfile"  || -z "$key" ]]; then
    echo "Usage: $0 <directory> <outfile> <key>";
    exit;
fi

if [[ ! -d "$directory" ]]; then
    echo "$directory is not a directory.";
    exit;
fi

if [[ -f "$outfile" ]]; then
    echo "$outfile already exists, aborting.";
    exit;
fi

if [[ ! -f "$directory/$hexfile" ]]; then
    echo "$hexfile is missing in $directory.";
    exit;
fi

if [[ ! -f "$directory/$metadata" ]]; then
    echo "$metadata is missing in $directory.";
    exit;
fi

mkdir -p "$tmpdir"
$ZIP "$zipfile" "$directory/$hexfile" "$directory/$metadata"

openssl dgst -sha256 -sign "$key" -out "$tmpfile" "$zipfile"
openssl enc -base64 -in "$tmpfile" -out "$sigfile"
rm "$tmpfile"

$ZIP "$outfile" "$zipfile" "$sigfile"
rm "$zipfile" "$sigfile"

rmdir "$tmpdir"
