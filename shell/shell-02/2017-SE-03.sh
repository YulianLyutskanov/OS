#!/bin/bash
if [[ "${USER}" == "s0600295" ]]; then
    processes=$(ps -e -o user=,pid=,rss= | tr -s ' ' )

    users=$( echo "${processes}" | cut -d ' ' -f 1 | sort | uniq )

    user_count_sum=$( while read user; do
        hisProc=$( echo "${processes}" | egrep "^${user} " )
        echo "${hisProc}" | awk -F ' ' -v user="${user}" ' BEGIN { all=0; sum=0 } { all+=1; sum+=$3} END { printf("%s %d %d\n",user,all,sum) } '

    done < <(echo "${users}") )

    while read line; do
        curUser=$( echo "${line}" | cut -d ' ' -f 1)
        curCount=$( echo "${line}" | cut -d ' ' -f 2)
        curSum=$( echo "${line}" | cut -d ' ' -f 3)

        echo "${curUser} ${curCount} ${curSum}"

        curAvg=$(echo " ${curSum} / ${curCount} " | bc -l | cut -d '.' -f 1)
        heaviest=$( echo "${processes}" | egrep "^${curUser} " | sort -t ' ' -k 3 -n  | tail -n 1 | cut -d ' ' -f 2,3)
        pid=$( echo "${heaviest}" | cut -d ' ' -f 1)
        rss=$( echo "${heaviest}" | cut -d ' ' -f 2)

        if [[ $( echo "2 * ${rss}" | bc -l) -gt $curAvg ]]; then
            kill -SIGKILL "${pid}"
        fi

    done < <(echo "${user_count_sum}" )

fi
