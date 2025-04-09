#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo " needed 1 arg"
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo " arg must be a dir"
    exit 2
fi

arch=$( find "${1}" -mindepth 1 | egrep "^[^_]*_report-[0-9]+\.tgz$" )

touch "${1}/checksums.txt"

new_hash=$(mktemp)

echo "${arch}" | xargs -I {} sha256sum "{}" | tr -s ' ' | sort -t ' ' -k 2 >> "${new_hash}"

joined=$( join -1 2 -2 2 -o 1.1,2.2,2.1 -a 2 "${1}/checksums.txt" "${new_hash}" )

modified=$( echo "${joined}" | awk -F ' ' ' (($1 != $3) && (NF == 3)) { print $2 }
                                            (NF == 2)                 { print $1 }  ' )

tdir=$(mktemp -d)

while read gzip; do
    if [[ -z "${gzip}" ]]; then
        continue
    fi

    tar -x -f "${gzip}" --one-top-level="${tdir}"

    files=$(find "${tdir}" | egrep "meow.txt$")
    while read file; do
        if [[ -z "${file}" ]]; then
            continue
        fi

        mkdir -p "${1}/extracted"

        filename=$( echo "${gzip}" | sed -E "s:^.*/([^_/]*)_report-([0-9]+)\.tgz:\1_\2.txt:g")

        cp "${file}" "${1}/extracted/${filename}"
        rm -r "${tdir}"/*
    done < <( echo "${files}" )

done < <(echo "${modified}")


cat "${new_hash}" > "${1}/checksums.txt"
rm "${new_hash}"
rm -r "${tdir}"
