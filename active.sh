#!/bin/bash


join -t ' ' -1 1 -2 1 -o 2.2 2.3 1.2 1.3 \
<(who | tr -s ' ' | cut -d ' ' -f 1,3,4 |sort | uniq -w 10) \
<(cut -d ':' -f 1,5 /etc/passwd | sort | cut -d ',' -f 1 | tr ':' ' ') | \
sort -t ' ' -k 3,4 \
| column -t -N NAME,"LAST NAME",LOGGED

exit 0
