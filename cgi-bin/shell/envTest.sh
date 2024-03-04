#!/bin/bash
echo "Content-Type: text/html"

echo ""
echo "<!DOCTYPE html>"
echo "<html><head>"
echo "    <title>CGI Script - Display Environment Variables</title>"
echo "    <style>html {color-scheme: light dark;}</style>"
echo "</head><body>"

echo "<h2>Environment Variables from Shell:</h2>"
echo "<h2>---------------------------------</h2>"
echo -n "<p>"

env_array=($(env | sort))

for element in "${env_array[@]}"; do
	# Check if the element contains an equal sign
	if [[ $element == *"="* ]]; then
		echo "<br>"  # Print without newline
	fi
	echo "$element"  # Print on a new line
done

echo "</body>\n</html>"
