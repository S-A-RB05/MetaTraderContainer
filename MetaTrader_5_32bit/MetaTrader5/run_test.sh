#!/bin/bash

# Set the path to the Metatrader 5 executable
MT5_EXE="terminal.exe"

# Set the path to the config file for the strategy test
CONFIG_FILE="Config\testconfig.ini"

# Start Metatrader 5 with the specified config file
wine $MT5_EXE "/config:$CONFIG_FILE" &

# Wait for the test report file to appear in the directory
while [ ! -f Report/tester_report.htm ]
do
    sleep 10
done

# Wait for an additional 10 seconds to ensure the file is completely written
sleep 10

# Stop MetaTrader 5
pkill -f 'terminal.exe'
