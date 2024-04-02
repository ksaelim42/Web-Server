#!/usr/bin/python3

import cgi, os, sys

form = cgi.FieldStorage()

if 'filename' in form:
	fileitem = form['filename']
else:
	print("filename not Found")
	sys.exit(1) # Exit Fail

dirUpload = "../upload/"

if fileitem.filename:
	file_name = os.path.basename(fileitem.filename)
	if os.path.exists(dirUpload + file_name):
		message = 'Files is already Exists'
	else:
		f = open(dirUpload + file_name, "wb") # 'wb' for write in binary mode
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
print("<a href=\"/cgi-bin/list.py\">Go Back</a>")
print("</body>\n</html>")
sys.exit(0) # Exit Success