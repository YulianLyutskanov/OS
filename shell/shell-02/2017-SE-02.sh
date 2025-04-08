#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo " 3 params needed"
    exit 1
fi

if [[ (! -d "${1}") || (! -d "${2}") ]]; then
    echo " arg1 and 2 must be directories"
    exit 2
fi

if [[ ! -z $(find "${2}" -mindepth 1 ) ]]; then
    echo " second dir must be empty"
    exit 3
fi

files=$(find "${1}" -mindepth 1 -type f | egrep "${3}")

while read filename; do
    dirsToMake=$(echo "${filename}" | sed -E "s:${1}\/(.*):\1:" | sed -E  "s:\/*[^/]*$::" | egrep '.+')

    while read dir; do
        mkdir -p "${2}/${dir}"
    done < <( echo "${dirsToMake}")

done < <( echo "${files}")

while read filename; do
    newName=$( echo "${filename}" | sed "s:^[^/]*/::")
    mv "${filename}" "${2}/${newName}"

done < <( echo "${files}")
