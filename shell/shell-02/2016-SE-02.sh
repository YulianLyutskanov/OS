#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "comandneed exactly one param"
    exit 1
fi

if [[ $1 -lt 0 ]]; then
    echo " the parameter needs to be a positive number"
    exit 2
fi


if [[ "s0600295" == "$(whoami)" ]]; then

    user_pid_rss=$(ps -e -o user=,pid=,rss=)
    active_users=$(echo -e "${user_pid_rss}" | cut -d ' ' -f 1 | sort | uniq)

    while read user; do
        curSum=0
        hisProc=$(echo "${user_pid_rss}" | egrep "^${user}" | tr -s ' ')

        while read proc; do
            curRSS=$(echo "${proc}" | cut -d ' ' -f 3)
            curSum=$(( curSum + curRSS ))
        done < <(echo "${hisProc}")
        echo "${user} ${curSum}"

        if [[ ${curSum} -gt ${1} ]]; then
            kill -SIGKILL $( echo "${hisProc}" | sort -t ' ' -k 3 -n | tail -n 1 | cut -d ' ' -f 2)
        fi
    done < <(echo "${active_users}")

fi
