### Executablem###
NAME	= webserv

### Compilation ###
CC		= c++
RM		= rm -f
CFLAGS	= #-Wall -Wextra -Werror -std=c++98

### Source Files ###
SRCS	= src/webserv.cpp src/cgi.cpp

### Object Files ###
OBJS	= $(SRCS:.cpp=.o)

### Compilation Rule ###
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re : fclean all

.PHONY: all clean fclean re