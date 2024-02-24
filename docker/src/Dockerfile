FROM	debian

# install nginx
RUN		apt update\
		&& apt install vim nginx -y

# install cgi shell
RUN		apt install spawn-fcgi fcgiwrap python3 -y

# copy nginx conf
COPY	./conf/nginx.conf /etc/nginx/conf.d/

# copy 
COPY	./tools/.bashrc /root

# copy run script
COPY	./tools/start_server.sh /root

CMD		[ "bash", "/root/start_server.sh" ]

# CMD [ "sleep", "infinity" ]

EXPOSE	1400 1410 1420