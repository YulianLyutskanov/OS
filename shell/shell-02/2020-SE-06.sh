#!/bin/bash

if [[ ! $# -eq 3 ]]; then
    echo "needs 3 args"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "first is't existing file"
    exit 2
fi


found=$(grep -E "^ *${2} *= *[a-zA-Z0-9_]+" ${1} | tr -s ' ' |sed 's/^ //')

if [[ ! -z ${found} ]]; then
    if [[ $(echo ${found} | cut -d ' ' -f 3 ) == ${3} ]]; then
        exit 0;
    else
        sed -E  "s/(^ *${2} *=.*$)/\
        #\0#edited at $(date) by $(whoami)\n${2}=${3}# added at $(date) by $(whoami)/" -i ${1}
    fi

else
    echo "${2}=${3}# added at $(date) by $(whoami)" >> ${1}
fi

exit 0
