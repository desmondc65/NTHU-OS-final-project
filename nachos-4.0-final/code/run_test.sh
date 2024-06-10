#!/bin/bash

# Step 4: Compile the Project
echo "Cleaning and Compiling the project..."
make clean
make
userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 80 4000 -d z t s
echo "All tests completed."
