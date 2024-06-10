#!/bin/bash

# Step 4: Compile the Project
echo "Cleaning and Compiling the project..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Compilation failed, stopping script."
    exit 1
fi

echo "Compilation successful."

# Step 5: Run tests with different parameters
echo "Running tests..."

# Define an array of test parameters
declare -a params=(
    "40 5000"
    "80 4000"
)

# Path to the test programs
test_program1="test/hw2_test1"
test_program2="test/hw2_test2"

# Loop through all parameter sets and execute the tests
for param in "${params[@]}"; do
    IFS=' ' read -r -a arr <<< "$param"
    p="${arr[0]}"
    bt="${arr[1]}"

    echo "Executing with parameters p=$p bt=$bt"
    ./userprog/nachos -epb $test_program1 $p $bt -epb $test_program2 $p $bt -d z
    echo "Test completed for parameters p=$p bt=$bt"
done

echo "All tests completed."
