### Executablem###
NAME	= mytest

### Directory ###
DIR_INCS	= includes
DIR_SRCS	= srcs
DIR_OBJS	= objs

### Compilation ###
CC		= c++
RM		= rm -f
CFLAGS	= #-Wall -Wextra -Werror -std=c++98
IFLAGS	= #-I$(DIR_INCS)

### Source Files ###
# HEADERS	= $(wildcard $(DIR_INCS)/*.hpp)

SRCS	= Server.cpp Utils.cpp parsingConfig.cpp configUtils.cpp

### Object Files ###
OBJS	= $(subst $(DIR_SRCS), $(DIR_OBJS), $(SRCS:.cpp=.o))

### Compilation Rule ###
$(DIR_OBJS)/%.o:$(DIR_SRCS)/%.cpp $(DIR_INCS)/Struct.hpp
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
# NAME	= mytest

# CC = g++
# CFLAGS = #-Wall -Wextra -std=c99
# SRC = parsingConfig.cpp Utils.cpp Server.cpp
# OBJ = $(SRC:.c=.o)

# $(NAME):$(OBJ)
# 	$(CC) $(CFLAGS) -o $@ $^

# %.o: %.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@

# all: $(NAME) 

# clean:
# 	rm -f $(OBJS)

# fclean: clean
# 	rm -f $(NAME)

# re : fclean all

# .PHONY: all clean fclean re