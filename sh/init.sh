#!/bin/bash

. ./config.sh

cd ..

arduino-cli compile --fqbn $BOARD --build-path ./build --only-compilation-database --libraries $LIBRARIES .

ln build/compile_commands.json compile_commands.json
