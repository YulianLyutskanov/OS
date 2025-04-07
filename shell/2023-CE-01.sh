#!/bin/bash


find / -user "$(whoami)" 2> /dev/null | egrep '.*\.blend[0-9]+$'
