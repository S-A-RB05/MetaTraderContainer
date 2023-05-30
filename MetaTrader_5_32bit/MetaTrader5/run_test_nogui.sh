#!/bin/bash

# Set the path to the Metatrader 5 executable
MT5_EXE="terminal.exe"

# Set the path to the config file for the strategy test
CONFIG_FILE="Config\config.ini"

# RabbitMQ Connection Details
RABBITMQ_URL="amqps://tnhdeowx:tInXH7wKtKdyn-v97fZ_HGM5XmHsDTNl@rattlesnake.rmq.cloudamqp.com/tnhdeowx"
QUEUE_NAME="mt5_test"

# Receive message from RabbitMQ
python3 receive.py $RABBITMQ_URL $QUEUE_NAME Config/config.ini

# Check if the config file exists
while [ ! -f Config/config.ini ]
do
    sleep 10
done

# Start Metatrader 5 with the specified config file
xvfb-run wine $MT5_EXE "/config:$CONFIG_FILE" &

# Wait for the test report file to appear in the directory
while [ ! -f Report/tester_report.htm ]
do
    sleep 10
done

# Wait for an additional 10 seconds to ensure the file is completely written
sleep 10

# Execute sender.py script
python3 format_csv.py

sleep 5

# Execute sender.py script
python3 sender.py

# Stop MetaTrader 5
pkill -f 'terminal.exe'

# Exit with a status code of 0 to signal success to Kubernetes
exit 0
