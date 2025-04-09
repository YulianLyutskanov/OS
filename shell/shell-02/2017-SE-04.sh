#!/bin/bash

if [[ ( ! "$#" -ne 1) && ( ! "$#" -ne 2 ) ]]; then
    echo "1 or 2 params"
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "1st must be dir"
    exit 2
fi

if [[ ("$#" -eq 2) && ( ! -f "$2") ]]; then
    echo " 2nd must be file"
fi

sl=$( find "$1" -type l  -printf "%f %l\n")

not_broken=$( while read syml; do
    src=$( echo "${syml}" | cut -d ' ' -f 1)
    dst=$( echo "${syml}" | cut -d ' ' -f 2)

    if [[ -e "${dst}" ]]; then
        echo "${src} -> ${dst}"
    fi
done < <(echo "${sl}" ) )

broken=$(find -L "$1" -type l | wc -l)

if [[ "$#" -eq 2 ]]; then
    echo "${not_broken}" >> "${2}"
    echo "Broken symlinks: ${broken}" >> "${2}"
else
    echo "${not_broken}"
    echo "Broken symlinks: ${broken}"
fi
