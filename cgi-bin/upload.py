#!/usr/bin/python3

import cgi, os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

# Get filename
fileitem = form['filename']

if fileitem.filename:
	file_name = os.path.basename(fileitem.filename)
	f = open("./upload/" + file_name, "wb") # 'wb' for write in binary mode
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