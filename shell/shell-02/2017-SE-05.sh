#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " 2 params needed"
    exit 1
fi


if [[ ! -d "$1" ]]; then
    echo "1st must be dir"
    exit 2
fi

find "$1" -mindepth 1 -maxdepth 1 -type f | xargs -I {}  basename "{}" |\
egrep "^vmlinuz-[1-9][0-9]*\.[1-9][0-9]*\.[1-9][0-9]*\-${2}" | sed -E "s/^vmlinuz-//g" | sed -E "s/-${2}$//g" |\
sort -t '.' -n | tail -n 1 | sed -E "s/^/vmlinuz-/g" | sed -E "s/$/-${2}/g"
