#!/bin/bash


sonnest_time=0
soonest_file=0
soonest_user=0

while read line; do
    user=$(echo "$line" | awk -F ':' '{ print $1 }')
    homedir=$(echo "$line" | awk -F ':' '{ print $2 }')

    curSoonest_user_file=$(find "${homedir}" -type f -printf "%C@ %p\n" 2> /dev/null | sort -t ' ' -r -n -k 1 | head -n 1 )
    curTime=$(echo "${curSoonest_user_file}" | cut -d ' ' -f 1 | cut -d '.' -f 1 )

    if [[ $curTime -gt $sonnest_time ]]; then
        sonnest_time=$curTime
        soonest_user=$user
        soonest_file=$(echo "${curSoonest_user_file}" | cut -d ' ' -f 2 )
    fi

done < <(cat /etc/passwd | cut -d ':' -f 1,6)

echo "The user that changed his file most recently is : "${soonest_user}" "
echo "The file is "${soonest_file}" , changed $(date -d "@${sonnest_time}") "

