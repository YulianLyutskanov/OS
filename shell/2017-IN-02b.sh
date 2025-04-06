#!/bin/bash

find ~ -mindepth 1 -maxdepth 2 -printf "%p %s bytes\n" | sort  -r -n -t ' ' -k 2 | head -n 5
