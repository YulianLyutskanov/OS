#!/bin/bash

if [[ "${USER}" != "s0600295" ]]; then
    echo "only root can execute"
    exit 1
fi

while read line; do

    user=$(echo "${line}" | awk -F ':' ' {print $1}')
    user_gid=$(id -g)
    homedir=$(echo "${line}" | awk -F ':' ' {print $2}')

    if [[ ! -d "${homedir}" ]]; then
        echo "${user}"
        continue;
    fi

    owner_homedir=$(stat "${homedir}" -c '%U')
    group_homedir=$(stat "${homedir}" -c '%g')
    perm_homedir=$(stat "${homedir}" -c '%a')

    if [[ ("${user}" == "${owner_homedir}") && ( $(( 0${perm_homedir} & 0200 )) -eq 0200 ) ]]; then
        :
    elif [[ ("${user_gid}" -eq "${group_homedir}") && ( $(( 0${perm_homedir} & 020 )) -eq 020 ) ]]; then
        :
    elif [[ $(( 0${perm_homedir} & 02 )) -eq 02 ]]; then
        :
    else
        echo "${user}"
    fi

done < <(cat /etc/passwd | cut -d ':' -f 1,6)
