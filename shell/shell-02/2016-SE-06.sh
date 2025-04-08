#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo " 1 arg needed"
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo " arg must be a file"
    exit 2
fi

cat "${1}" | awk -F ' ' '{ printf("%d. %s\n",NR,$0)} ' | sed -E 's/(^[0-9]*.) [0-9]{4} .\. -/\1/g' | sort -t ' ' -k 2
