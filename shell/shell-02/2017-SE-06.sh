#!/bin/bash

if [[ "${USER}" != "s0600295" ]]; then
    echo " can be started only by root"
    exit 1
fi

proc=$(ps -e -o user=,pid=,rss= | tr -s ' ')
users=$( echo "${proc}" | egrep "^root " -v | cut -d ' ' -f 1 | sort | uniq)


matching_users=$( while read user; do
    homeDir=$( cat /etc/passwd | egrep "^${user}:" | cut -d ':' -f 6)

    if [[ ! -d "${homeDir}" ]]; then
        echo "${user}"
        continue
    fi

    if [[ $(stat "${homeDir}" -c %U) != "${user}" ]]; then
        echo "${user}"
        continue
    fi

    if [[ $(stat "${homeDir}" -c %A | cut -c 3) == "-" ]]; then
        echo "${user}"
        continue
    fi

done < <( echo "${users}") )


echo "${matching_users}"
root_rss=$( echo "${proc}" | egrep "^root " | awk -F ' ' ' BEGIN { sum = 0 } { sum+=$3 } END { print sum }' )
echo "${root_rss}"

while read user; do
    sumRSS=$( echo "${proc}" | egrep "^${user} " | awk -F ' ' 'BEGIN { sum = 0 } { sum+=$3 } END { print sum }' )
    echo "$sumRSS"
    if [[ ${sumRSS} -gt "${root_rss}" ]]; then
        killall -SIGKILL -u "${user}"
    fi

done < <(echo "${matching_users}" )
