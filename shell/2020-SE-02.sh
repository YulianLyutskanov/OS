#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "exactly 1 argument needed"
    exit 1
fi

if [[ ! -f $1 ]]; then
    echo "argument must be an existing file"
    exit 2
fi

most_failed_launch_site=$(cat "${1}" | tail +2 | egrep "^([^|]*\|){2}Failure" | cut -d '|' -f 2 | sort | uniq -c | tr -s ' ' | sed 's/^ //' | sort -n | cut -d ' ' -f 2)

cat "${1}" | tail +2 | egrep "^[^|]*\|${most_failed_launch_site}" | sort -t '|' -k 1 -r -n | head -n 1 | cut -d '|' -f 3,4 | sed 's/|/:/'
