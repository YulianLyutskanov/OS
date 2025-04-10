#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo " 1 arg needed " 1>&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo " config file needed" 1>&2
    exit 2
fi


while read line; do
    trim=$( echo "${line}" | sed -E "s/#.*//g"i | sed -E "s/\t/ /g"|\
    tr -s [:space:] | sed -E "s/^ //g" |  sed -E "s/ $//g")

    if [[ -z "${trim}" ]]; then
        continue
    fi

   dev=$( echo "${trim}" | cut -d ' ' -f 1 )
   option=$( echo "${trim}" | cut -d ' ' -f 2 )
   echo "$dev $option"

    if [[ -z $( cat "example-wakeup" | egrep " *${dev} ") ]]; then
        echo " warning : dev ${dev} doesn't exist " 1>&2
        continue;
    fi

    sed -E "s/( *${dev}[ ]+[A-Z0-9]{2,4}[ ]+\*)(enabled|disabled) /\1${option} /" -i "example-wakeup"

