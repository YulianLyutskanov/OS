#!/bin/bash

find ~ -type f -printf "%s %p\n" | sort -n -r | head -n 5 | cut -d ' ' -f 2- | xargs -I {} rm -i {}
