NAME = Var
SRCS = src/main.c
SRCS += src/array.c \
	src/error.c \
	src/strings.c
OBJS = $(SRCS:.c=.o)
ID = includes
CC = gcc
LD = gcc
CFLAGS = -Wall -Wextra -I$(ID) -g -fsanitize=address
LDFLAGS = 

all: $(NAME)

$(NAME): $(OBJS)
	$(LD) -o $(NAME) $(OBJS) $(CFLAGS) $(LDFLAGS)

%.o: %.c $(HDRS)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
