SRCS = main.cpp\
		ft_tools.cpp\
		theUsers.cpp\
		theChannels.cpp\

OBJS = ${SRCS:.cpp=.o}

NAME = ircserv

HEADER = includes

CC = c++
RM = rm -f

CFLAGS = -std=c++98 -Wall -Werror -Wextra -g -fsanitize=address

%.o: %.cpp
	${CC} -c $(CFLAGS) -o $@ $<

all: $(NAME)

$(NAME): ${OBJS}
	$(CC) ${CFLAGS} $(OBJS) -o $(NAME)

clean:
	@make -s
	@rm -rf $(OBJS)

fclean:	clean
	@rm $(NAME)
	
re: fclean all

.PHONY: all clean fclean re