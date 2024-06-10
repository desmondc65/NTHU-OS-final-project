#!/bin/bash

# Step 4: Compile the Project
echo "Cleaning and Compiling the project..."
make clean
make
userprog/nachos -epb test/hw2_test1 130 5000 -epb test/hw2_test2 120 4000 -d z 
echo "All tests completed."
