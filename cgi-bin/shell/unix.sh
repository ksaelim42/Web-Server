#!/bin/bash
if [ $REQUEST_METHOD = "GET" ]
then
	FORM_INFO=$QUERY_STRING
elif [ $REQUEST_METHOD = "POST" ]
then
	read -n $(CONTENT_LENGTH) FORM_INFO
fi

# split 
IFS="=" read -r KEY COMMAND <<< "$FORM_INFO"

if [ $COMMAND = "ls" ]
then
	BUFFER=$(ls)
elif [ $COMMAND = "pwd" ]
then
	BUFFER=$(pwd)
else
	BUFFER="$COMMAND : Commmand not allow"
fi

echo "Content-Type: Text/plain"

echo ""

echo $BUFFER
