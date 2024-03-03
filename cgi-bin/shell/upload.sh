#!/bin/bash

TARGET_DIR="../upload"

echo "content-type: text/html"
echo ""

echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "<title>File Upload</title>"
echo "</head>"
echo "<body>"

if [ "$REQUEST_METHOD" = "POST" ]; then
	read -n "$CONTENT_LENGTH" BUFFER

	FILE_NAME="HelloMama"
	FILE_PATH="$TARGET_DIR/$FILE_NAME"
	echo "--- From CGI script --- size: ${CONTENT_LENGTH}" 1>&2
	echo -e "${BUFFER}" 1>&2

	echo $BUFFER >"FILE_PATH"

	echo "<h2>File uploaded successfully: $FILE_NAME</h2>"

else
	echo "<h2>File uploaded fail</h2>"
fi

echo "<h1>Hello</h1>"

echo "</body>"
echo "</html>"
