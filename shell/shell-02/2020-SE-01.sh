#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " 2 args needed"
    exit 1
fi

if [[ ( -f "$1" ) || ( ! -d "$2" ) ]]; then
    echo " first must be non existing file, second existing dir"
    exit 2
fi


touch "$1"
echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" >> "$1"


while read file; do
    name=$(echo "${file}" | xargs -I {} basename "{}" | sed -E "s/.[a-zA-Z0-9]*$//g")
    rest=$(cat "${file}" | tr -s ' ' | sed -E 's/^ //g' | cut -d $'\n' -f 3-9,11,13,15 | sed -E "s/.*: //g" |\
    sed -E "s/This platform has a[n]{0,1} (.*) license./\1/g" | sed -E -z "s/[[:space:]]+/,/g" | sed -E "s/,$//g")

    echo "$name,$rest" >> "$1"
done < <(find "$2" -mindepth 1 | egrep '.*\.log' )
