#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo " 1 arg needed"
    exit 1
fi

if ! id "${1}" &>/dev/null; then
    echo "user with that name doesn't exist"
    exit 2
fi


if [[ "${USER}" == "root" ]]; then
    all_proc=$(ps -e -o user=,pid=,etimes=)
    user_proc_count=$( echo "${all_proc}" | egrep "^${1} " | wc -l)


    echo "Users with more processes than "${1}" : "
    echo "${all_proc}" | cut -d ' ' -f 1 | sort | uniq -c |\
    tr -s ' ' | sed -e 's/^ //g' | awk -F ' ' -v n="${user_proc_count}" ' $1 > n { print $2 } '

    avg_seconds=$(echo "${all_proc}" | tr -s ' ' | sed -e 's/^ //g' |\
    awk -F ' ' ' BEGIN { all=0; secs=0 } { all+=1; secs+=$3 } END { printf("%.3f", secs/all) } ')

    echo "Average time of all processes :   ${avg_seconds}"

    long_proc=$(  echo "${all_proc}" | egrep "^${1} " | tr -s ' ' | sed -e 's/^ //g' |\
    awk -F ' ' -v limit=${avg_seconds} ' $3*2 > limit { print $2 }')


    if [[ ! -z $long_proc ]]; then
         echo "${long_proc}" | xargs kill -KILL
    fi
fi
