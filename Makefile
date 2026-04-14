NAME = codexion
CFLAGS = -Wall -Wextra -Werror
CC = cc

FILES = coders/dongles_and_coders.c coders/heap.c coders/main.c coders/parse_tools.c coders/utilities.c
# coders/dongles_and_coders.c  coders/heap.c  coders/main.c  coders/parse_tools.c  coders/utilities.c

OBJ = $(FILES:.c=.o)
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

$(B_NAME): $(B_OBJ)
	$(CC) $(B_OBJ) -o $(B_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
fclean: clean
	rm -f $(NAME)
re: fclean all