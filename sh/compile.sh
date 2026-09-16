#!/bin/bash

. ./config.sh

cd ..

CLEAN_FLAG=""
if [ "$1" = "clean" ]; then
    CLEAN_FLAG="--clean"
fi

arduino-cli compile --fqbn $BOARD --libraries $LIBRARIES --output-dir ./build $CLEAN_FLAG .