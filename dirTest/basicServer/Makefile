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

1: $(NAME_CLIENT)
	$(CC) basicServe.cpp ServerTool.cpp -o $(NAME_SERVER)

2: $(NAME_CLIENT)
	$(CC) selectServe.cpp ServerTool.cpp -o $(NAME_SERVER)

$(NAME_CLIENT):
	$(CC) client.cpp -o $(NAME_CLIENT)

clean:
	$(RM) $(NAME_CLIENT) $(NAME_SERVER)

.PHONY: all clean