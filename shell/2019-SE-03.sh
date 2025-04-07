#!/bin/bash


velins_home=$(grep '^velin' /etc/passwd | cut -d ':' -f 6)

inod_last=$(find ${velins_home}  -printf "%T@ %i %d\n" 2>/dev/null |sort -t ' ' -k 1 -n | tail -n 1 | cut -d ' ' -f 2)

find ${velins_home} -inum ${inod_last} -printf "%T@ %i %d\n" 2>/dev/null | sort -t ' ' -k 3 -n | head -n 1 | cut -d ' ' -f 3
