### Executablem###
NAME_SERVER	= server
NAME_CLIENT	= client
LOOP_CLIENT	= loopClient

### Colors ###
YEL		= \e[0;33m
RESET	= \e[0m

### Compilation ###
CC		= c++
RM		= rm -f

all:
	@echo "Please put Argument"
	@echo "1 : basic server"
	@echo "2 : basic receive select server"
	@echo "3 : select server"
	@echo "4 : poll server"
	@echo "5 : epoll server"
	@echo "6 : non-blocking i/o server"
	@echo "9 : persistent connection server"

1: $(NAME_CLIENT)
	@$(CC) basicServe.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create Basic server\n$(RESET)"

2: $(NAME_CLIENT)
	@$(CC) basicSelect.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create Select with only receive server\n$(RESET)"

3: $(NAME_CLIENT)
	@$(CC) select.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create recv & send select server\n$(RESET)"

4: $(NAME_CLIENT)
	@$(CC) poll.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create Poll server\n$(RESET)"

5: $(NAME_CLIENT)
	@$(CC) epoll.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create Epoll server\n$(RESET)"

6: $(LOOP_CLIENT) # non-blocking i/o for recv & send Server
	@$(CC) nonblocking.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create non-blocing i/o server\n$(RESET)"

9: $(LOOP_CLIENT)
	@$(CC) epollPersisCon.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create persist-connection server\n$(RESET)"

$(NAME_CLIENT):
	@$(CC) basicClient.cpp -o $(NAME_CLIENT)

$(LOOP_CLIENT):
	@$(CC) loopClient.cpp -o $(LOOP_CLIENT)

clean:
	$(RM) $(NAME_CLIENT) $(LOOP_CLIENT) $(NAME_SERVER)

.PHONY: all clean