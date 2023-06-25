#!/bin/bash

# Set the IRC server, port, password, username, and nickname information
SERVER="localhost"
PORT=8080
PASSWORD="pass"
USERNAME="reda"
NICKNAME="red"
CHANNEL="#ch"

# Set the number of messages to send
NUM_MESSAGES=1000

# Set the message content
MESSAGE="This is a flooded message."

# Connect to the IRC server
(
  sleep 1
  echo "$PASSWORD"
  sleep 1
  echo "$USERNAME"
  sleep 1
  echo "$NICKNAME"
) | nc $SERVER $PORT &

# Store the process ID of the nc command
NC_PID=$!

# Wait for the connection to establish
sleep 2

# Join the channel
echo "/JOIN $CHANNEL"

# Flood the channel with messages
for ((i=1; i<=NUM_MESSAGES; i++))
do
    echo "/PRIVMSG $CHANNEL :$MESSAGE $i"
done

# Disconnect from the server
echo "/QUIT"

# Terminate the nc process
kill $NC_PID
