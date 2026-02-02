#!/bin/bash

#!/bin/bash

# Check if the first argument (commit message) is provided
if [ -z "$1" ]; then
  echo "Error: Commit message required."
 
  exit 1
fi

# Stage all changes
git add .

# Commit with the provided message
git commit -m "$1"

# Push to the current branch
git push

