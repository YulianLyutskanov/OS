#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo " 3 args needed"
    exit 1
fi

if [[ (! -f "$1" ) || ( -e "$2" ) || (! -d "$3") ]]; then
    echo "first must be an existing file, second non existing filename, 3rd dir"
    exit 2
fi

files=$( find "$3" -type f | egrep ".cfg$" )
broken=$(
while read file; do

    bn=$( basename "${file}" | sed 's/.cfg$//g')
    cat "${file}" | awk -F ' ' -v n="${bn}" ' BEGIN { c=0 } !(  $0 ~ /#.*|{.*};/) { c+=1 }
                                                      ( c== 1 )       { printf("Error in %s.cfg\n",n); c+=1 }
                                                      !(  $0 ~ /#.*|{.*};/)  { printf("Line %d:%s\n",NR,$0) } '

done < <( echo "${files}" ) )

echo "${broken}"
broken=$( echo "${broken}" | egrep ".cfg$" | sed -E 's/^Error in //g' )

fine="${files}"

while read br; do

    fine=$( echo "${fine}" | grep -v "${br}" )

done < <(echo "${broken}" )
echo "${fine}" | xargs -I {} cat "{}" >> "$2"
name=$( echo "$2" | sed 's/.cfg$//g' )

if [[ -z $(cat "$1" | egrep "^${name}" ) ]]; then
    pass=$(pwgen 16 1)
    echo "$name:$(echo "$pass" | md5sum | sed -E "s/[^0-9a-f]//g")" >> "$1"
    echo "$name:$pass"
fi
