#!/bin/bash

homeDir=$(egrep --color '^human' /etc/passwd | cut -d ':' -f 6)


find "${homeDir}" -links +1 -type f -printf "%i:%p:%TF-%TH-%TM-%TS\n" 2> /dev/null |\
sort -r -t ':' -k 3 |\
head -n 1 |\
cut -d : -f 1
