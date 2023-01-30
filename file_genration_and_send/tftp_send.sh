#!/bin/bash

# Loop 20 times
for i in {0..19}
do
    # Build the command string
    command="echo -e \"binary\\nput file$i.txt\\nquit\" | tftp 192.168.1.2"

    # Execute the command
    eval "$command"

    sleep 0.1

    # Delete the file
    rm "file$i.txt"
done
