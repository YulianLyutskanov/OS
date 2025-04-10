#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " 2 args needed"
    exit 1
fi

if [[ (! -d "${1}" ) || ( -e "${2}" ) ]]; then
    echo "first arg must be an existing dir, and the second is created from the script dir"
    exit 2
fi

while read -d $'\0' pic; do
    if [[ -z  $( echo "${pic}" | egrep "\.jpg$" ) ]]; then
        continue;
    fi

    title=$( echo "${pic}" | xargs -I {} basename "{}" | sed -E "s/.jpg$//g" | sed -E "s/\([^\(\)]*\)//g" |\
    tr -s ' ' | sed -E "s/^ //g" | sed -E "s/ $//g" )

    album=$( echo "${pic}" | xargs -I {} basename "{}" | sed -E "s/.jpg$//g" | sed -E "s/.*(\([^\(\)]*\))[^\(\)]*/\1/g" |\
    sed -E "s/^[^\(\)]*$/misc/g" | sed -E "s/\(//g" | sed -E "s/\)//g" )

    date=$( stat "${pic}" --printf "%y\n" | cut -d ' ' -f 1 )

    cs=$( sha256sum "${pic}" | cut -c 1-16 )

    mkdir -p "${2}/images"
    cp "${pic}" "${2}/images/$cs.jpg"

    mkdir -p "${2}/by-date/$date/by-album/$album/by-title"
    ln -s "$(realpath "${2}")/images/$cs.jpg" "${2}/by-date/$date/by-album/$album/by-title/$title.jpg"

    mkdir -p "${2}/by-date/$date/by-title"
    ln -s "$(realpath "${2}")/images/$cs.jpg" "${2}/by-date/$date/by-title/$title.jpg"

    mkdir -p "${2}/by-album/$album/by-date/$date/by-title"
    ln -s "$(realpath "${2}")/images/$cs.jpg" "${2}/by-album/$album/by-date/$date/by-title/$title.jpg"

    mkdir -p "${2}/by-album/$album/by-title"
    ln -s "$(realpath "${2}")/images/$cs.jpg" "${2}/by-album/$album/by-title/$title.jpg"

    mkdir -p "${2}/by-title"
    ln -s "$(realpath "${2}")/images/$cs.jpg" "${2}/by-title/$title.jpg"

