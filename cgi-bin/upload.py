#!/usr/bin/python3

import cgi, os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

# Get filename
fileitem = form['filename']

if fileitem.filename:
	file_name = os.path.basename(fileitem.filename)
	f = open("../myPage/upload/" + file_name, "wb") # 'wb' for write in binary mode
	f.write(fileitem.file.read())
	message = 'The file "' + file_name + '" was uploaded successfully'
	f.close()
else:
	message = 'No file was uploaded'

print("Content-Type: text/html")

print("")

print("<!DOCTYPE html>")
print("<html><head>")
print("<title>Upload files</title>")
print("<style>html")
print("{color-scheme: light dark;}")
print("body { display: flex; justify-content: center; flex-direction: column ; align-items: center; height: 20vh; margin: 0;}")
print("a { text-decoration: none; padding: 10px 20px; background-color: #505050; color: #a0a0a0; border-radius: 7px;}")
print("a:hover {background-color: lightgrey; text-decoration: underline; }")
print("</style>")
print("</head><body>")
print("<h2>" + message + "</h2>")
print("<a href=\"/cgi.html\">Go Back</a>")
print("</body>\n</html>")