#!/usr/bin/python3

import cgi, os
import cgitb; cgitb.enable()

import sys # for debug

form = cgi.FieldStorage()

# Get filename
fileitem = form['filename']

if fileitem.filename:
	file_name = os.path.basename(fileitem.filename)
	f = open("cgi-bin/upload/" + file_name, "wb")
	f.write(fileitem.file.read())
	message = 'The file "' + file_name + '" wa uploaded successfully'
else:
	message = 'No file was uploaded'

print("Content-Type: text/html")

print("")

print("<!DOCTYPE html>")
print("<html><head>")
print("<title>Upload files</title>")
print("<style>html {color-scheme: light dark;}</style>")
print("</head><body>")
print("<h1>" + message + "</h1>")
print("</body>\n</html>")