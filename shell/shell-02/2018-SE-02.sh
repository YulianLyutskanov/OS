#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " 2 params needed"
    exit 1
fi

if [[ ( ! -f "$1" ) || ( ! -d "$2" ) ]]; then
    echo " first must be file and the second - dir"
    exit 2
fi

if [[ $( find "$2" -mindepth 1 -maxdepth 1 | wc -l ) -ne 0 ]]; then
    echo " the dir must be empty"
    exit 3
fi

ppl=$( cat "$1" | sed -E "s/^( )*([a-zA-Z-]+( )+[a-zA-Z-]+)( )*.*/\2/g" | sed -E "s/( )+/ /g" | sort | uniq )
ppl_num=$( echo "${ppl}" | awk -F ' ' ' { printf("%s ;%d\n", $0, NR) } ' )
echo "${ppl_num}" >> "${2}/dict.txt"

while read p_n; do
    first_name=$( echo "${p_n}" | cut -d ' ' -f 1 )
    second_name=$( echo "${p_n}" | cut -d ' ' -f 2 | cut -d ';' -f 1 )
    num=$( echo "${p_n}" | cut -d ' ' -f 3 | cut -d ';' -f 2 )
    lines=$( cat "$1" | egrep "^( )*${first_name}( )+${second_name}( )*(\(.*\)){0,1}( )*:" )

    echo "${lines}" >> "${2}/${num}.txt"

done < <( echo "${ppl_num}" )
