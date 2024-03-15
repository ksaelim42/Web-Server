#!/usr/bin/python3

import os

print("Content-Type:text/html")

print("")

print("<!DOCTYPE html>")
print("<html><head>")
print("<title>Environment Variables</title>")
print("<style>html")
print("{color-scheme: light dark;}")
print("li {list-style: none;}")
print("a { text-decoration: none; padding: 10px 20px; background-color: #303030; color: #404040; border-radius: 7px;}")
print("a:hover {background-color: lightgrey; text-decoration: underline; }")
print("</style>")
print("</head><body>")
print("<h2>Environment Variables</h2>")
print("<ul>")
for key in sorted(os.environ):
	print("<li>{} : {}</li>".format(key, os.environ[key]))
print("</ul>")
print("<a href=\"/cgi.html\">Go Back</a>")
print("</body>\n</html>")