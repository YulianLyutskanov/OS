#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " 2 params needed"
    echo 1
fi

if [[ (! -f "$1" ) || ( -f "$2" ) ]]; then
    echo " first must be an existing file, second must not exist"
    exit 2
fi

no_id=$( cat "$1" | sed -E 's/^[1-9]+[0-9]*//g' | sort | uniq)

while read line; do
    cat "$1" | egrep "^[1-9]+[0-9]*${line}" | sort -n | head -n 1 >> "$2"
done < <( echo "${no_id}" )
