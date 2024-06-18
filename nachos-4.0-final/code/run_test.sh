#!/bin/bash

# Step 4: Compile the Project
echo "Cleaning and Compiling the project..."
make clean
make

case "$1" in
    1)
        echo "Running Test Case 1..."
        userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 80 4000 -d z
        ;;
    2)
        echo "Running Test Case 2..."
        userprog/nachos -epb test/hw2_test1 40 5000 -epb test/hw2_test2 40 4000 -d z
        ;;
    3)
        echo "Running Test Case 3..."
        userprog/nachos -epb test/hw2_test1 90 5000 -epb test/hw2_test2 100 4000 -d z
        ;;
    *)
        echo "Invalid test case number. Please specify 1, 2, or 3."
        exit 1
        ;;
esac

echo "All tests completed."
