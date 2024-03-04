#!/usr/bin/php8.2

<?php

echo "Content-type: text/html\n\n";
echo "<!DOCTYPE html>\n";
echo "<html>\n<head>\n";
echo "    <title>CGI Script - Display Environment Variables</title>\n";
echo "    <style>html {color-scheme: light dark;}</style>\n";
echo "</head>\n<body>\n";

echo "<h2>Environment Variables from PHP:</h2>\n";
ksort($_SERVER);

// Print all environment variables
foreach ($_SERVER as $key => $value) {
    echo "<strong>$key:</strong>";

    // Check if the value is an array
    if (is_array($value)) {
        echo "<pre>" . print_r($value, true) . "</pre>";
    } else {
        echo "$value <br>";
    }
}

echo "</body>\n</html>";
?>
