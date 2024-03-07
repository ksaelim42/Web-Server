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
	@echo "2 : basic receive select server"
	@echo "3 : select server"
	@echo "4 : poll server"

# @echo "4 : non-blocking i/o server"

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

# 6: $(NAME_CLIENT) # non-blocking i/o for recv & send Server
# 	@$(CC) nonBlockServe.cpp ServerTool.cpp -o $(NAME_SERVER)
# 	@printf "$(YEL)Create non-blocing i/o server\n$(RESET)"

$(NAME_CLIENT):
	@$(CC) client.cpp -o $(NAME_CLIENT)

clean:
	$(RM) $(NAME_CLIENT) $(NAME_SERVER)

.PHONY: all clean