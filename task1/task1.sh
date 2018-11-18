#!/bin/bash

. ../config-run.sh --source-only

DEFAULT_NUMBER_OF_PROCESSES=3

run "1" ${DEFAULT_NUMBER_OF_PROCESSES} $1