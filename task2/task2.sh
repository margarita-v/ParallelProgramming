#!/bin/bash

. ../config-run.sh --source-only

DEFAULT_NUMBER_OF_PROCESSES=2

run "2" ${DEFAULT_NUMBER_OF_PROCESSES} $1