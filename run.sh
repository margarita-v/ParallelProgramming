#!/bin/bash

USAGE="Usage: run -t <task-number> -p <number-of-processes> [--help]"
INFO="Run task with defined number of processes"
ERROR="Error: Invalid parameters!"
NUMBER_EXP="^[1-9]+([0-9])?$"

print_info() {
	echo
	echo ${USAGE}
	echo ${INFO}
	echo
}

print_error() {
    echo
    echo ${ERROR}
    print_info
    exit 1
}

check_argument() {
    if [[ -z "$1" ]] || ! [[ $1 =~ $NUMBER_EXP ]]; then
        return 1
    else
        return 0
    fi
}

unset TASK_NUMBER NUMBER_OF_PROCESSES

if [[ $# -gt 1 && $1 -eq "--help" ]]; then
    print_info
elif [[ $# -lt 4 ]]; then
    print_error
else
    while getopts ':t:p:' opt; do
        case "$opt" in
            t) TASK_NUMBER="$OPTARG" ;;
            p) NUMBER_OF_PROCESSES="$OPTARG" ;;
            *) print_error ;;
        esac
    done
    if ! check_argument "$TASK_NUMBER" || ! check_argument "$NUMBER_OF_PROCESSES"; then
        print_error
    else
        #todo run task
        echo task "$TASK_NUMBER" num "$NUMBER_OF_PROCESSES"
    fi
fi
