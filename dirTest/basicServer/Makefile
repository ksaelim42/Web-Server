### Executablem###
NAME_SERVER	= server
NAME_CLIENT	= client

### Colors ###
YEL		= \e[0;33m
RESET	= \e[0m

### Compilation ###
CC		= c++
RM		= rm -f

all:
	@echo "Please put Argument"
	@echo "1 : basic server"
	@echo "2 : select server"
	@echo "3 : send&recv select server"
	@echo "4 : non-blocking i/o server"

1: $(NAME_CLIENT) # basic Server can send & recv between Server - Client
	@$(CC) basicServe.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create Basic server\n$(RESET)"

2: $(NAME_CLIENT) # select function for only recv 
	@$(CC) selectReadServe.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create Select server\n$(RESET)"

3: $(NAME_CLIENT) # select function for recv & send
	@$(CC) selectRWServe.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create recv & send select server\n$(RESET)"

4: $(NAME_CLIENT) # non-blocking i/o for recv & send Server
	@$(CC) nonBlockServe.cpp ServerTool.cpp -o $(NAME_SERVER)
	@printf "$(YEL)Create non-blocing i/o server\n$(RESET)"

$(NAME_CLIENT):
	@$(CC) client.cpp -o $(NAME_CLIENT)

clean:
	$(RM) $(NAME_CLIENT) $(NAME_SERVER)

.PHONY: all clean