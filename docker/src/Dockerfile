FROM	debian:bullseye

ARG		DB_NAME \
		DB_USER \
		DB_PASSWORD

# install nginx
RUN		apt update\
		&& apt install vim nginx -y

# install cgi shell
RUN		apt install fcgiwrap -y

# copy nginx conf
COPY	./conf/nginx.conf /etc/nginx/conf.d/

# copy mariadb script
COPY	./tools/mariadb.sh /home/

# copy wordpress script
COPY	./tools/wordpress.sh /home/

# copy run script
COPY	./tools/start_server.sh ./home/

RUN		bash /home/mariadb.sh

CMD		[ "bash", "/home/start_server.sh" ]

EXPOSE	120