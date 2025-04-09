#!/bin/bash

if [[ ($# -ne 1) && (! -d "$1") ]]; then
    echo " needed 1 arg - dir"
    exit 1
fi


friends=$( find "$1" -type f |\
egrep "${1}(/[^/]*){3}/([0-9]){4}-([0-9]){2}-[0123][0-9]-[012][0-9]-[012345][0-9]-[012345][0-9].txt$" |\
xargs dirname |sed -E 's/\/$//g' | xargs -I {} basename "{}" | sort | uniq)

res=$(  while read fr; do
    sessions=$( find "$1" -type f |\
     egrep "${1}(/[^/]*){2}/${fr}/([0-9]){4}-([0-9]){2}-[0123][0-9]-[012][0-9]-[012345][0-9]-[012345][0-9].txt$")

    count=0
    while read ses; do
        count=$(( count + $( cat "${ses}" | wc -l ) ))
    done < <(echo "${sessions}" )
    echo "${fr} ${count}"

done < <( echo "${friends}" )  )

echo "${res}" | sort -t ' ' -k 2 -n -r | head -n 10
