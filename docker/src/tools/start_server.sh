#!bin/bash

service fcgiwrap start

service php8.2-fpm start

nginx -g "daemon off;"
