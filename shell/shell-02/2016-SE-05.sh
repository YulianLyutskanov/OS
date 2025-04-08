#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " 2 params needed"
    exit 1
fi


if [[ (! -f "$1" ) || (! -f "$2") ]]; then
    echo " params should be files"
    exit 2
fi

f1_count=$(cat "$1" | egrep "${1}" | wc -l )
f2_count=$(cat "$2" | egrep "${2}" | wc -l )

winner=0

if [[ f1_count -lt f2_count ]]; then
    winner="${2}"
else
    winner="${1}"
fi
echo "${winner}"
cat "${winner}" | sed -E "s/^[0-9]{4}.. ${winner} - //g" | sort >> ""${winner}".songs"
