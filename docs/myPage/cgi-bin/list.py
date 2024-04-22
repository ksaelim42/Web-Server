#!/usr/bin/python3

import os

def convert_size(size_bytes):
	if size_bytes == 0:
		return "0 Bytes"
	size_name = ("Bytes", "KB", "MB", "GB", "TB")
	i = size_bytes
	p = 0
	while i >= 1024:
		i = i / 1024
		p += 1
	if p == 0:
		return "{} Bytes".format(i)
	return "{:.1f} {}".format(i, size_name[p])

directory = "../upload"

boxIcon = """<svg aria-hidden=\"true\" height=\"16\" viewBox=\"0 0 16 16\" version=\"1.1\" width=\"16\" data-view-component=\"true\" class=\"octicon octicon-package color-fg-muted\">
<path fill=\"#E0E0E0\" d=\"m8.878.392 5.25 3.045c.54.314.872.89.872 1.514v6.098a1.75 1.75 0 0 1-.872 1.514l-5.25 3.045a1.75 1.75 0 0 1-1.756 0l-5.25-3.045A1.75 1.75 0 0 1 1
 11.049V4.951c0-.624.332-1.201.872-1.514L7.122.392a1.75 1.75 0 0 1 1.756 0ZM7.875 1.69l-4.63 2.685L8 7.133l4.755-2.758-4.63-2.685a.248.248 0 0 0-.25 0ZM2.5
 5.677v5.372c0 .09.047.171.125.216l4.625 2.683V8.432Zm6.25 8.271 4.625-2.683a.25.25 0 0 0 .125-.216V5.677L8.75 8.432Z\"></path>
</svg>"""

mylist = []

found = True

if os.path.exists(directory) and os.path.isdir(directory):
	for filename in os.listdir(directory):
		href = " href=\"/upload/" + filename + "\""
		dowload = " download=\"" + filename + "\""
		dataUrl = "http://" + os.environ["HTTP_HOST"] + "/upload/" + filename
		sublist = []
		sublist.append("<li>" + boxIcon + "&nbsp<a" + href + dowload + ">" + filename + "</a>" + "</li>")
		size = os.path.getsize(directory + "/" + filename)
		sublist.append("<p>" + convert_size(size) + "</p>")
		sublist.append("<button class=\"del-but\" type=\"button\" onclick=\"deleteResource(this)\" data-url=\"" + dataUrl + "\" data-filename=\"" + filename + "\">Delete</button>")
		mylist.append(sublist)
else:
	found = False

print("Content-Type: text/html")

print("")

print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("<meta charset=\"UTF-8\">")
print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("<link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\">")
print("<link rel=\"stylesheet\" href=\"/css/style.css\">")
print("<link rel=\"stylesheet\" href=\"/css/listStyle.css\">")
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
		for listItem in mylist:
			print("<div class=\"container-list\">")
			print("<div class=\"one\">" + listItem[0] + "</div>")
			print("<div class=\"list-line two\">" + listItem[1] + "</div>")
			print("<div class=\"list-line three\">" + listItem[2] + "</div>")
			print("</div>")
		print("</ul>")
		print("</div>") # class fill-box
	else:
		print("<p style=\"color: #A0A0A0; text-align: center;\">Directory is empty</p>")
else:
	print("<p>Error: Directory not found</p>")
print("</div>") # class body-box
# Upload scope
print("<div class=\"body-box\">")
print("<form action=\"/cgi-bin/upload.py\" method=\"post\" enctype=\"multipart/form-data\">")
print("<h3>Upload Files</h3>")
print("<div class=\"fill-box\" style=\"margin-bottom: 5px;\">")
print("File: <input type=\"file\" id=\"filename\" name=\"filename\" />")
print("</div>")
print("<input class=\"button\" type=\"submit\" value=\"Upload\" />")
print("</form>")
print("</div>")
print("</div>") # class body
print("<script src=\"/js/myscript.js\"></script>")
print("</body>")
print("</html>")
