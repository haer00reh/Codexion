NAME = codexion
CFLAGS = -Wall -Wextra -Werror -pthread -fsanitize=thread
CC = clang

FILES = coders/coder_cycle.c coders/coder_routine.c coders/dongle_access.c \
	coders/dongles_and_coders.c \
	coders/heap.c coders/heap_utils.c coders/main.c coders/monitor.c \
	coders/parse_tools.c coders/simulation_entities.c coders/simulation_init.c \
	coders/utilities.c

OBJ = $(FILES:.c=.o)
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -fsanitize=thread -fno-PIE -no-pie -o $(NAME)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
fclean: clean
	rm -f $(NAME)
re: fclean all