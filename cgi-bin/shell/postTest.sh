#!/bin/bash

# ! Post method will read the data as size of Content-length
read -n $(CONTENT_LENGTH) BUFFER

echo $BUFFER
