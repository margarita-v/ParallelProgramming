#!/bin/bash

NUMBER_EXP="^[1-9]+([0-9])?$"

check_argument() {
    : '
        Function which is checking if the argument is not null and is number
    '
    if [[ -z $1 || ! $1 =~ $NUMBER_EXP ]]; then
        return 1
    else
        return 0
    fi
}

get_task_filename() {
    : '
        Function for searching a task filename by task number
    '
    find $1 -name "*.cpp" -print -quit 2>/dev/null
}