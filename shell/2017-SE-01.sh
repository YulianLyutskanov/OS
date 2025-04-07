#!/bin/bash

find . -mindepth 1 -maxdepth 1 -printf "%p %n\n" |\
sort -t ' ' -k 2 -n -r |\
head -n 5
