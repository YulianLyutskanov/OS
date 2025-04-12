#!/bin/bash


occ="$(echo "$0" | xargs realpath | sed -E "s:[^/]*$::g" )occ"

list=$("${occ}" user:list | sed -E "s/^- ([^:]*):.*/\1/g" )
locals=$(cat /etc/passwd | cut -d ':' -f 1,3 | awk -F ':' ' $2 >= 1000 { printf ("%s\n",$1) } ')

while read loc; do
    cur=$(echo "${list}" | egrep "^${loc}" )
    if [[ -z "${cur}" ]]; then
        "${occ}" user:add "${loc}"
    else
        if [[ -z $( "${occ}" user:info "${loc}" | egrep "- enabled: true") ]]; then
            "${occ}" user:enable "${loc}"
        fi
    fi

done < <(echo "${locals}")

while read not_local; do
    if [[ -z $( echo "${locals}" | egrep "^${not_local}" ) ]]; then

        if [[ -z $( "${occ}" user:info "${not_local}" | egrep "- enabled: false") ]]; then
            "${occ}" user:disable "${not_local}"
        fi

    fi

done < <(echo "${list}")
