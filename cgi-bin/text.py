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
print("<style>html {color-scheme: light dark;}</style>")
print("</head><body>")
print("<h1>Test Text Field!</h1>")
print("<p><strong>First name</strong>:", first_name, "</p>")
print("<p><strong>Last name</strong> :", last_name, "</p>")
print("</body>\n</html>")
