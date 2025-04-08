#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo " 3 args needed"
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo " first must be existng file "
    exit 2
fi

value_1=$(cat "${1}" | egrep "^${2}=" | sed -E "s/^${2}=//g")

if [[ $( cat "${1}" | egrep -o "^${3}=" | wc -l ) -eq 0 ]]; then
    echo "${3}=" >> "${1}"
    exit 0
fi

while read -d ' ' term; do
    sed -E "s/(^${3}=.*)${term}/\1/" -i "${1}"
done < <(echo "${value_1}")
