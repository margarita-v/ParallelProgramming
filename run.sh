#!/bin/bash

: '
    Main script for all tasks
'

USAGE="Usage: run -t <task-number> -p <number-of-processes> [--help]"
INFO="Run task with defined number of processes"
ERROR="Error: Invalid parameters!"
NUMBER_EXP="^[1-9]+([0-9])?$"

# List of files for all tasks
FIRST_TASK_FILENAME=./task1/task1_num5.cpp
SECOND_TASK_FILENAME=./task2/task2_num4.cpp

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
    : '
        Function which is checking if the argument is not null and is number
    '
    if [[ -z "$1" ]] || ! [[ $1 =~ $NUMBER_EXP ]]; then
        return 1
    else
        return 0
    fi
}

unset TASK_NUMBER NUMBER_OF_PROCESSES

# Checking for number of arguments
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
        unset CURRENT_TASK_FILENAME
        case "$TASK_NUMBER" in
            1) CURRENT_TASK_FILENAME=${FIRST_TASK_FILENAME} ;;
            2) CURRENT_TASK_FILENAME=${SECOND_TASK_FILENAME} ;;
            *) print_error ;;
        esac

        # Task solve
        OUT_FILE_NAME=out/task"$TASK_NUMBER"
        mpic++ -o "$OUT_FILE_NAME" ${CURRENT_TASK_FILENAME}
        mpirun -np "$NUMBER_OF_PROCESSES" ./"$OUT_FILE_NAME"
    fi
fi
