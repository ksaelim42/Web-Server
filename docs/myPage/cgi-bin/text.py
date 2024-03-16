#!/usr/bin/python3

# import module from CGI handling
import cgi, cgitb

# create Instance fo FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('fname')
last_name = form.getvalue('lname')

# Response Message
print("Content-Type: text/html")

print("")

print("<!DOCTYPE html>")
print("<html><head>")
print("<title>CGI Script Text Field</title>")
print("<style>html")
print("{color-scheme: light dark;}")
print("body { display: flex; justify-content: center; flex-direction: column ; align-items: center; height: 80vh; margin: 0;}")
print("a { text-decoration: none; padding: 10px 20px; background-color: #303030; color: #404040; border-radius: 7px;}")
print("a:hover {background-color: lightgrey; text-decoration: underline; }")
print("h1 { margin: 5px;}")
print("</style>")
print("</head><body>")
print("<h1>Hello &#128522</h1><br>")
print("<h1>", first_name + "&nbsp&nbsp" + last_name , "</h1><br>")
print("<a href=\"/cgi.html\">Go Back</a>")
# print("<p><strong>First name</strong>:", first_name, "</p>")
# print("<p><strong>Last name</strong> :", last_name, "</p>")
print("</body>\n</html>")