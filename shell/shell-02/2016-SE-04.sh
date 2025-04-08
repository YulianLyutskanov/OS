#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "script needs exactly 2 params"
    exit 1;
fi

if [[ ( ! "${1}" =~ ^[0-9]{0,1}[0-9]*$ ) || ( ! "${2}" =~ ^[0-9]{0,1}[0-9]*$ ) ]]; then
    echo "params must be positive numbers"
    exit 2
fi

mkdir -p 'a'
mkdir -p 'b'
mkdir -p 'c'

while read -d $'\0' filename; do
    wc=$(cat "${filename}" | wc -l)

    if [[ $wc -lt $1 ]]; then
        mv "${filename}" ./a
    elif [[ $wc -lt $2  &&  $wc -gt $1 ]]; then
        mv "${filename}" ./b
    else
        mv "${filename}" ./c
    fi

done < <(find . -mindepth 1  -maxdepth 1 -type f -print0 )
