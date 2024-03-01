#!/bin/bash

echo "content-type: text/html"

echo ""

if [ "$REQUEST_METHOD" = "GET" ]; then
    FORM_INFO=$QUERY_STRING
else # POST method
    read -n $CONTENT_LENGTH FORM_INFO
    echo "----------------------------------size : $CONTENT_LENGTH" 1>&2
    echo "-------------------------------content : $FORM_INFO" 1>&2
fi

FIRST_NAME=$(echo "$FORM_INFO" | cut -d'&' -f1 | cut -d'=' -f2)
LAST_NAME=$(echo "$FORM_INFO" | cut -d'&' -f2 | cut -d'=' -f2)

echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "<title>CGI Script Text Field</title>"
echo "<style>html {color-scheme: light dark;} p {font-size: 1.3rem;}</style>"
echo "</head>"
echo "<body>"

echo "<h1>Test Text Field!</h1>"
echo "<p> <strong>First name</strong>: $FIRST_NAME</p>"
echo "<p> <strong>Last  name</strong>: $LAST_NAME</p>"

echo "</body>"
echo "</html>"
