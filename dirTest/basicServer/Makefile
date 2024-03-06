### Executablem###
NAME_SERVER	= server
NAME_CLIENT	= client

### Compilation ###
CC		= c++
RM		= rm -f

all:
	@echo "Please put Argument"
	@echo "1 : basic server"
	@echo "2 : select server"

1: $(NAME_CLIENT) # basic Server can send & recv between Server - Client
	$(CC) basicServe.cpp ServerTool.cpp -o $(NAME_SERVER)

2: $(NAME_CLIENT) # select function for only recv 
	$(CC) selectReadServe.cpp ServerTool.cpp -o $(NAME_SERVER)

3: $(NAME_CLIENT) # select function for recv & send
	$(CC) selectRWServe.cpp ServerTool.cpp -o $(NAME_SERVER)

$(NAME_CLIENT):
	$(CC) client.cpp -o $(NAME_CLIENT)

clean:
	$(RM) $(NAME_CLIENT) $(NAME_SERVER)

.PHONY: all clean