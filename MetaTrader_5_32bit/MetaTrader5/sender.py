import os
import json
import pika
import csv

# Connection parameters
params = pika.URLParameters("amqps://tnhdeowx:tInXH7wKtKdyn-v97fZ_HGM5XmHsDTNl@rattlesnake.rmq.cloudamqp.com/tnhdeowx")

# Establish a connection to RabbitMQ
connection = pika.BlockingConnection(params)
channel = connection.channel()

# Declare the queue
channel.queue_declare(queue="mt5_results")

# Path to the CSV file to send
file_path = "Report/output.csv"

# Read the CSV file and convert it to a string
csv_string = ""
if os.path.isfile(file_path):
    with open(file_path, "r") as file:
        csv_reader = csv.reader(file)
        csv_data = list(csv_reader)
        csv_string = "\n".join([",".join(row) for row in csv_data])

# Get the value of the JOB_ID environment variable
job_id = os.environ.get("JOB_ID")

# Create the message dictionary
message = {
    "job_id": job_id,
    "csv_data": csv_string
}

# Convert the message to JSON
message_json = json.dumps(message)

# Publish the message to the queue
channel.basic_publish(exchange="", routing_key="mt5_results", body=message_json)

# Close the connection
connection.close()

print("Message sent to the 'mt5_results' queue.")
