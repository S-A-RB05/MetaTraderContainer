import pika
import sys

def receive_message(url, queue_name, output_file):
    # Establish connection to RabbitMQ
    connection = pika.BlockingConnection(pika.URLParameters(url))
    channel = connection.channel()

    # Declare the queue
    channel.queue_declare(queue=queue_name)

    def callback(ch, method, properties, body):
        # Save the received file contents to the output file
        with open(output_file, 'wb') as file:
            file.write(body)

        # Stop consuming messages after receiving one
        channel.stop_consuming()

    # Set up a consumer to receive messages
    channel.basic_consume(queue=queue_name, on_message_callback=callback, auto_ack=True)

    # Start consuming messages
    channel.start_consuming()

    # Close the connection
    connection.close()
    
if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: python receive.py [RabbitMQ_URL] [queue_name] [output_file]")
        sys.exit(1)

    url = sys.argv[1]
    queue_name = sys.argv[2]
    output_file = sys.argv[3]

    receive_message(url, queue_name, output_file)
