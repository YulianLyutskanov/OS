#!/bin/bash

n=10
flag=5
if [[ "${1}" == "-n" ]]; then
    n="${2}"
    flag=1
fi


for file in "${@}"; do
    if [[ "${flag}" -lt 3 ]]; then
      flag=$(( flag + 1))
      continue
    fi

    idf=$( echo "${file}" | sed -E 's/\.log$//g' | sed -E 's:^.*\/::g')
    cat -- "${file}" |\
    sed -E "s/(^[1-9][0-9]{3}-[01][0-9]-[0123][0-9] [012][0-9]\:[012345][0-9]\:[012345][0-9] )/\1${idf} /g" |\
    tail -n "${n}"

done | sort
