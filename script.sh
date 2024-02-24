#!/bin/bash

# Example strings
string1="variable=value"
string2="just_a_value"

# Check if a string contains an equal sign
if [[ $string1 == *"="* ]]; then
    echo "$string1 contains an equal sign."
else
    echo "$string1 does not contain an equal sign."
fi

if [[ $string2 == *"="* ]]; then
    echo "$string2 contains an equal sign."
else
    echo "$string2 does not contain an equal sign."
fi