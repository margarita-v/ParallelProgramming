#!/bin/bash

. ../config-run.sh --source-only

DEFAULT_NUMBER_OF_PROCESSES=2

run "2" ${DEFAULT_NUMBER_OF_PROCESSES} $1 <<< "0 1 0.00001 5" #todo for tests only
