#!/bin/bash

find /home/students/s0600295/sample-data/01-shell/2023-CE-02/my_logs -maxdepth 1 -mindepth 1 -type f |\
egrep '/[a-zA-Z0-9_]+_[0-9]+.log$' | xargs -I {} cat "{}" | egrep -o 'error' | wc -l
