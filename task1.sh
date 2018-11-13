#!/bin/bash

NUMBER_EXP="^[1-9]+([0-9])?$"
DEFAULT_NUMBER_OF_PROCESSES=3

run() {
    ./run.sh -t 1 -p "$1"
}

if [[ -z "$1" ]] || ! [[ $1 =~ $NUMBER_EXP ]]; then
    run "$DEFAULT_NUMBER_OF_PROCESSES"
else
    run "$1"
fi