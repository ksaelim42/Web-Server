### Executablem###
NAME	= webserv

### Directory ###
DIR_INCS	= includes
DIR_SRCS	= srcs
DIR_OBJS	= objs

### Compilation ###
CC		= c++
RM		= rm -f
CFLAGS	= -Wall -Wextra -Werror -std=c++98
IFLAGS	= -I$(DIR_INCS)

### Source Files ###
HEADERS	= $(wildcard $(DIR_INCS)/*.hpp)

SRCS	= $(wildcard $(DIR_SRCS)/*.cpp)

### Object Files ###
OBJS	= $(subst $(DIR_SRCS), $(DIR_OBJS), $(SRCS:.cpp=.o))

### Compilation Rule ###
$(DIR_OBJS)/%.o:$(DIR_SRCS)/%.cpp $(HEADERS) $(DIR_INCS)/Webserv.hpp
	@mkdir -p $(DIR_OBJS)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

all: $(NAME) 

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)
	$(RM) -r $(DIR_OBJS)

fclean: clean
	$(RM) $(NAME)

re : fclean all

.PHONY: all clean fclean re