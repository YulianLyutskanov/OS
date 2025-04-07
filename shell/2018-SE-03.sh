#!/bin/bash

group=$(cat /etc/passwd | sort -t ':' -k 3 | head -n 201 | tail -n 1 | cut -d ':' -f 4)

egrep --color "^([^:]*:){3}${group}" /etc/passwd | sort -t ':' -n -k 1 | cut -d ':' -f 5,6 | sed 's/,.*:/:/g'
