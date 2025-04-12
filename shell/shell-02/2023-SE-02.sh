#!/bin/bash

if [[ $# -lt 2 ]]; then
    echo " needed at least 2 args"
    exit 2
fi


if [[ ! "$1" =~ ^[1-9][0-9]*$ ]]; then
    echo "first arg should be num"
    exit 1
fi

start=$(date +%s.%N)
limit="$1"
shift 1

count=0

while [[ $( echo "($(date +%s.%N) - ${start}) < ${limit}" | bc -l ) == "1" ]]; do
    "${@}"
    count=$(("${count}" + 1))
done

end=$(date +%s.%N)
total=$(echo "$( echo "${end} - ${start}" | bc -l )" | awk '{printf("%.2f",$1)}' )
avg=$(echo "${total}/${count}" | bc -l | awk '{printf("%.2f",$1)}' )

echo "Ran the command '${@}' ${count} times for ${total} seconds."
echo "Average runtime: ${avg} seconds."
