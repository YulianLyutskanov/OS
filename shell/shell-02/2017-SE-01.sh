#!/bin/bash

if [[  ! ( $# -eq 1 || $# -eq 2 ) ]]; then
    echo "arguments must be 1 or 2"
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "first argument must be an existing directory"
    exit 2
fi

if [[ ($# -eq 2) && ( ! "$2" =~ ^[0-9]{0,1}[0-9]$ ) ]]; then
    echo "second optional param must be a number"
    exit 3
fi


if [[ $# -eq 1 ]]; then

    find -L "$1"  -type l

elif [[ $# -eq 2 ]]; then
    find "$1" -links +$2

fi
