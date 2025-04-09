#!/bin/bash


function sumDigits {
    echo "${1}" | sed -E 's/-//g' | sed -E 's/([0-9])/\1+/g' | sed -E 's/\+$//g' | bc -l
}

function max {
    if [[ "${1}" -gt "${2}" ]]; then
        echo "${1}"
    else
        echo "${2}"
    fi
}


tf=$(mktemp)
echo "${tf}"

while read line; do

    echo "${line}" | egrep "^-*[1-9][0-9]*$" >> "${tf}"

done

max_v=0
while read num; do
    max_v=$( max "${max_v}" $(sumDigits "${num}"))
done < <( cat "${tf}" )

while read num; do
    if [[ $(sumDigits "${num}") -eq "${max_v}" ]]; then
        echo "${num}"
    fi
done < <( cat "${tf}" ) | sort -n | uniq

rm "${tf}"
