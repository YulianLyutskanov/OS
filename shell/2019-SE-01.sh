#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "one argument needed"
    exit 1
fi

if [[ ! -f "${1}" ]] then
    echo "given argument should be file"
    exit 2
fi

furthest=$(cat "${1}" | tail -n +2 |  sort -t ';' -k 3,3 -rn | head -n 1)

f_type=$(echo "${furthest}" | cut -d ';' -f 2)

cat "${1}" | tail -n +2 | egrep "${f_type}" | sort -t ';' -k 3,3 -n | head -n 1 | cut -d ';' -f 1,4 --output-delimiter=$'\t'
