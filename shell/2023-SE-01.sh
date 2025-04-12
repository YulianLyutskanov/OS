#!/bin/bash

 if [[ $# -ne 2 ]]; then
    echo " 2 args needed"
    exit 1
fi


if [[ ( ! -f "$1"  ) || ( ! -d "$2" ) ]]; then
    echo " firs must be file, second dir"
    exit 2
fi


to_change=$( find "$2" -type f | egrep ".*\.txt$")

while read file; do

    while read sw; do
        len=$(( $( echo "${sw}" | wc -c) - 1 ))
        replacement=$( for c in $(seq 1 "$len"); do
            echo -n "*"
        done)
        sed -E "s/\b$sw\b/${replacement}/gi" -i "${file}"

    done < <(cat "$1" )

done < <(echo "${to_change}")
