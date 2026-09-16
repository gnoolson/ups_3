#!/bin/bash

. ./config.sh

arduino-cli monitor -p $PORT --config baudrate=9600
