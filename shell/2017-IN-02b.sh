#!/bin/bash

find ~ -printf "%p %s bytes\n" | sort  -r -n -t ' ' -k 2 | head -n 5 | cut -d ' ' -f 1 | xargs -I {} rm -i {}
