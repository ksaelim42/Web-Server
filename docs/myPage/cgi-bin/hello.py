#!/usr/bin/python3

import sys, time
# print("run cgi-script hello", file=sys.stderr)
time.sleep(10)

print("Content-Type: text/html")

print("")

print("<!DOCTYPE html>")
print("<html><head>")
print("<title>CGI Script Say Hello</title>")
print("<style>html")
print("{color-scheme: light dark;}")
print("body { display: flex; justify-content: center; flex-direction: column ; align-items: center; height: 80vh; margin: 0;}")
print("a { text-decoration: none; padding: 10px 20px; background-color: #303030; color: #808080; border-radius: 7px;}")
print("a:hover {background-color: lightgrey; text-decoration: underline; }")
print("</style>")
print("</head><body>")
print("<h1>Hello, I'm from Python script!</h1><br>")
print("<a href=\"/cgi.html\">Go Back</a>")
print("</body>\n</html>")

