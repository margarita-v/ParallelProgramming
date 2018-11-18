#!/bin/bash

: '
    Config file for easy call task solve
'

NUMBER_EXP="^[1-9]+([0-9])?$"

run() {
    CURRENT_DIR=`pwd`
    cd ..

    TASK_NUMBER=$1
    DEFAULT_NUMBER_OF_PROCESSES=$2
    CURRENT_NUMBER_OF_PROCESSES=$3

    RUN_COMMAND="./run.sh -t ${TASK_NUMBER} -p "

    if [[ -z ${CURRENT_NUMBER_OF_PROCESSES} ]] || \
        ! [[ ${CURRENT_NUMBER_OF_PROCESSES} =~ $NUMBER_EXP ]]
    then
        RUN_COMMAND+=${DEFAULT_NUMBER_OF_PROCESSES}
    else
        RUN_COMMAND+=${CURRENT_NUMBER_OF_PROCESSES}
    fi

    exec ${RUN_COMMAND}
    cd ${CURRENT_DIR}
}