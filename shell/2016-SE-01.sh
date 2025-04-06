#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "script needs only 1 argument!"
    exit 1
fi

if [[ ! -f $1 ]]; then
    echo "input must be existing file!"
    exit 2
fi

grep -E '^[^a-w]*[02468]+[^a-w]*$' ${1} | wc -l
