#!/bin/bash


if [[ $# -ne 1 ]]; then
    echo " needs exactly 1 argument"
    exit 1;
fi

if [[ ! -d "${1}" ]]; then
    echo " arg should be directory"
    exit 2
fi


find -L "${1}" -type l
