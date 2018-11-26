#!/bin/bash

: '
    Main script for all tasks
'

. utils.sh --source-only

USAGE="Usage: run -t <task-number> -p <number-of-processes> [--help]"
INFO="Run task with defined number of processes"
ERROR="Error: Invalid parameters!"

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
        if [[ $? != 0 || -z ${TASK_FILENAME} ]]; then
            print_error
        else
            mpic++ -o ${OUT_FILE_NAME} --std=c++11 ${TASK_FILENAME}
            mpirun -np ${NUMBER_OF_PROCESSES} ./${OUT_FILE_NAME}
        fi
    fi
fi
