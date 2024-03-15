#!/usr/bin/python3

import os

directory = "upload"

mylist = []

found = True

if os.path.exists(directory) and os.path.isdir(directory):
	for filename in os.listdir(directory):
		mylist.append("<li>" + filename + "</li>")
else:
	found = False

print("Content-Type: text/html")

print("")

print("<!DOCTYPE html>")
print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("<meta charset=\"UTF-8\">")
print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("<link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">")
print("<link rel=\"stylesheet\" href=\"/css/style.css\">")
print("<title>Files Uploaded</title>")
print("</head>")
print("<body>")
print("<nav class=\"navbar\">")
print("<h1 class=\"one\">Welcome to My cat website</h1>")
print("<div class=\"two\">")
print("<ul class=\"first\">")
print("<li class=\"nav-box\"><a href=\"/index.html\">Home</a></li>")
print("<li class=\"nav-box\"><a href=\"/blog\">AutoIndex</a></li>")
print("<li class=\"nav-box\"><a href=\"/blog/subdir/findme.html\">Redirection</a></li>")
print("<li class=\"nav-box\"><a href=\"/cgi.html\">Test CGI</a></li>")
print("</ul>")
print("<ul class=\"second\">")
print("<li class=\"nav-box\"><a href=\"/cgi-bin/list.py\">Files Uploaded</a></li>")
print("</ul>")
print("</div>")
print("</nav>")
print("<div class=\"body\">")
print("<h2 style=\"text-align: center;\">List Files</h2>")
print("<div class=\"body-box\">")
if found:
	if len(mylist):
		print("<div class=\"fill-box\">")
		print("<ul>")
		for x in mylist:
			print(x)
		print("</ul>")
		print("</div>")
	else:
		print("<p>Directory is empty</p>")
else:
	print("<p>Error: Directory not found</p>")
print("</div>")
print("</div>")
print("</form>")
print("</div>")
print("</div>")
print("</body>")
print("</html>")

