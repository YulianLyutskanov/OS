#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo " 1 param needed"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo " arg must be file"
    exit 2
fi

top3=$( cat "${1}" | sed -E 's/^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3} //g' |\
cut -d ' ' -f 1 | sort | uniq -c | tr -s ' ' |  sed -E 's/^ //g' | sort -t ' ' -k 1 -n | tail -n 3 | cut -d ' ' -f 2 )

while read site; do
    lines=$( cat "${1}" | egrep "^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3} ${site} - " )

    echo "${lines}" | awk -F ' ' -v s="${site}" ' BEGIN { http2=0; not2=0 }
                                                 ( $8 == "HTTP/2.0" ) { http2+=1 }
                                                 ( $8 != "HTTP/2.0" ) { not2+=1 }
                                                  END { printf("%s HTTP/2.0: %d  non-HTTP/2.0: %d\n",s,http2,not2) } '

done < <( echo "${top3}" )

clients=$(cat "${1}" | cut -d ' ' -f 1 | sort | uniq )

while read cl; do
    cat "${1}" | awk -F ' ' -v c="${cl}" ' BEGIN { count=0 }
                                           ( $1 == c ) && ( $9 > 302) { count+=1 }
                                           END { printf("%d %s\n",count,c) } '

done < <( echo "${clients}" )  | sort -t ' ' -k 1 -n -r | head -n 5
