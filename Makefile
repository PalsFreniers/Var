NAME = Var
SRCS = src/main.c
SRCS += src/array.c \
	src/error.c \
	src/setup.c \
	src/tokens.c \
	src/logger.c \
	src/strings.c
CC = clang
LD = clang
CFLAGS = -Wall -Wextra -std=gnu2x -Isrc
LDFLAGS = 

ifdef DEBUG
	CFLAGS += -g -fsanitize=address
endif
ifdef RELEASE
	CFLAGS += -Werror -O3
	LDFLAGS += -flto
endif

all: $(NAME)

$(NAME): $(SRCS)
	$(LD) -o $(NAME) $(SRCS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
