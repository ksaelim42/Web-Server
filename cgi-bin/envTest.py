#!/usr/bin/python3

import os

print("Content-Type:text/html")

print("")

print("<!DOCTYPE html>")
print("<html><head>")
print("<title>Environment Variables</title>")
print("<style>html {color-scheme: light dark;}</style>")
print("</head><body>")
print("<h1>Environment Variables</h1>")
print("<ul>")
for key in sorted(os.environ):
	print("<li><strong>{}</strong>: {}</li>".format(key, os.environ[key]))
print("</ul>")
print("</body>\n</html>")