#!/bin/bash

# Check if a commit message was provided
if [ -z "$1" ]; then
  echo "Usage: $0 <commit_message>"
  exit 1
fi

# Assign the first argument as the commit message
COMMIT_MESSAGE=$1

# Add all changes
git add .

# Commit with the provided message
git commit -m "$COMMIT_MESSAGE"

# Push to the remote repository
git push origin

# Print a success message
echo "Changes have been committed and pushed successfully."
