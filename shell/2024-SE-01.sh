#!/bin/bash

if [[ $# -lt 2 ]]; then
    echo "needed 2 args at least" 1>&2
    exit 3
fi

reps=$( mktemp)
file=""
for arg in "${@}"; do

    if [[ $( echo "$arg" | cut -c 1 ) == "-" ]]; then

        if [[ $( echo "$arg" | cut -c 2) == "R"  ]]; then
            echo "$arg" | cut -c 3- >> "${reps}"
            continue
        else
            echo " only -R flag allowed" 1>&2
            exit 4
        fi
    else

        if [[ ! -z "$file" ]]; then
            echo " you can give only one filename " 1>&2
            exit 1;
        elif [[ ! -f "$arg" ]]; then
            echo " this is not existing file" 1>&2
            exit 2
        else
            file="$arg"
        fi
    fi
done

if [[ -z "$file" ]]; then
    echo " you must give file" 1>&2
    exit 4
fi

key1_to_change=$(pwgen 5 1)
key2_changed=$(pwgen 5 1)

while read word; do
    from=$( echo "${word}" | cut -d '=' -f 1)
    sed -E "s/${from}/${key1_to_change}${from}/g" -i "${file}"
done < <(cat "${reps}")


while read word; do
    from=$( echo "${word}" | cut -d '=' -f 1)
    to=$( echo "${word}" | cut -d '=' -f 2)
    sed -E "s/${key1_to_change}${from}/${key2_changed}${to}/g" -i "${file}"

done < <(cat "${reps}")


sed -E "s/${key2_changed}//g" -i "${file}"


rm "$reps"
