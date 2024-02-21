#!/bin/bash
echo "Content-Type: Text/plain"

echo ""

echo "Environment Variables:"
echo "----------------------"

# Iterate through all environment variables and display them
for var in $(env | cut -d= -f1); do
	echo "$var=${!var}"
done