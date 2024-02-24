FROM	debian:bookworm

# install nginx
RUN		apt update\
		&& apt install vim nginx procps tree -y

# install cgi shell
RUN		apt install spawn-fcgi fcgiwrap -y

# install cgi python3
RUN		apt install python3 -y

# install cgi php
RUN		apt install php8.2 php8.2-fpm -y

# copy nginx conf
COPY	./conf/nginx.conf /etc/nginx/conf.d/

# copy bashrc
COPY	./tools/.bashrc /root

# copy vimrc
COPY	./tools/.vimrc /root

# copy run script
COPY	./tools/start_server.sh /root

CMD		[ "bash", "/root/start_server.sh" ]

# CMD [ "sleep", "infinity" ]

EXPOSE	1400 1410 1420