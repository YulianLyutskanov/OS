#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo " needed 2 args"
    exit 1
fi

if [[ (! -d "$1" ) || (! -d "$2") ]]; then
    echo " 2 directories needed"
    exit 2
fi

tempdir=$(mktemp -d)

pack_name=$( basename "$2" )
pack_version=$( cat "$2"/version )
tar -c -f "${tempdir}/tree.tar"  "$2/tree"
xz -k "${tempdir}/tree.tar"
cs=$( sha256sum "${tempdir}/tree.tar.xz"  | cut -d ' ' -f 1)
vername=$( echo "$pack_name-$pack_version")


csdb=$(cat "$1/db" | awk -F ' ' -v vn="${vername}" ' ($1 == vn ) { print $2 } ' )
if [[ ! -z "${csdb}" ]]; then
    rm "$1/packages/${csdb}.tar.xz"
    sed -E "s/^$vername $csdb$/${vername} ${cs}/g" -i "$1/db"
else
    echo "${vername} ${cs}" >> "$1/db"
fi

mv "${tempdir}/tree.tar.xz" "$1/packages/${cs}.tar.xz"

rm -r "${tempdir}"
