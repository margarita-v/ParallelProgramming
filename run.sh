#!/bin/bash

: '
    Main script for all tasks
'

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
    : '
        Function which is checking if the argument is not null and is number
    '
    if [[ -z $1 ]] || ! [[ $1 =~ $NUMBER_EXP ]]; then
        return 1
    else
        return 0
    fi
}

get_task_filename() {
    : '
        Function for searching a task filename by task number
    '
    find $1 -name "*.cpp" 2>/dev/null
}

# Checking for number of arguments
if [[ $# -gt 1 && $1 -eq "--help" ]]; then
    print_info
elif [[ $# -lt 4 ]]; then
    print_error
else
    unset TASK_NUMBER NUMBER_OF_PROCESSES
    while getopts ':t:p:' opt; do
        case ${opt} in
            t) TASK_NUMBER=$OPTARG ;;
            p) NUMBER_OF_PROCESSES=$OPTARG ;;
            *) print_error ;;
        esac
    done
    if ! check_argument ${TASK_NUMBER} || ! check_argument ${NUMBER_OF_PROCESSES}; then
        print_error
    else
        # Task solve
        TASK_FOLDER_NAME=task${TASK_NUMBER}
        OUT_FILE_NAME=out/${TASK_FOLDER_NAME}

        TASK_FILENAME=`get_task_filename ${TASK_FOLDER_NAME}`
        if [[ $? != 0 ]]; then
            print_error
        else
            mpic++ -o ${OUT_FILE_NAME} ${TASK_FILENAME}
            mpirun -np ${NUMBER_OF_PROCESSES} ./${OUT_FILE_NAME}
        fi
    fi
fi
