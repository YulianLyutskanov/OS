#!/bin/bash

 egrep --color 'Inf' ../sample-data/01-shell/2017-IN-03/example-passwd |\
 egrep --color '^[^:]*:[^:]*:[^:]*:[^:]*:[a-zA-Z ]*a[,:]' |\
 cut -d ':' -f 1 | sed -E 's/[^0-9]//g' | cut -c 2,3 | sort | uniq -c | tr -s ' ' | sort -n -r | head -n 1
