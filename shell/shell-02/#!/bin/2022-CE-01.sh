#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo " 3 args needed"
    exit 1
fi

mult=$( cat "prefix.csv" | tail -n +2 | egrep "^[^,]*,${2},[^,]*$" | cut -d ','  -f 3 )
unit=$( cat "base.csv" | tail -n +2 |  egrep "^[^,]*,${3},[^,]*$" | awk -F ',' '{ printf("(%s, %s)", $3, $1) }' )
echo "$(echo "${1}*${mult}" | bc -l) $3 $unit "
