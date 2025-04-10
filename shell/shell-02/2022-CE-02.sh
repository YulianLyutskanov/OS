#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo " 1 devcie name needed"
    exit 1
fi

sed -E "s/(^ +${1} +[A-Z0-9]+ +\*)(enabled|disabled)( +)/\1disabled\3/ "  -i "example-wakeup"
