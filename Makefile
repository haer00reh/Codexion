NAME = codexion

CFLAGS = -Wall -Wextra -Werror -pthread -fsanitize=thread
CC = clang

FILES = coders/coder_cycle.c coders/coder_routine.c coders/dongle_access.c \
	coders/dongles_and_coders.c \
	coders/heap.c coders/heap_utils.c coders/main.c coders/monitor.c \
	coders/parse_tools.c coders/simulation_entities.c coders/simulation_init.c \
	coders/utilities.c

OBJ = $(FILES:.c=.o)

TOTAL := $(words $(FILES))
COUNT := 0

# Colors
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
RED = \033[0;31m
RESET = \033[0m

define progress_bar
	$(eval COUNT=$(shell echo $$(($(COUNT)+1))))
	$(eval PERCENT=$(shell echo $$(($(COUNT)*100/$(TOTAL)))))
	@printf "\r$(BLUE)[BUILD]$(RESET) [%-50s] %d%% (%d/%d)" \
	"$$(printf '█%.0s' $$(seq 1 $$(($(PERCENT)/2)))))" \
	$(PERCENT) $(COUNT) $(TOTAL)
endef

all: intro $(NAME)

intro:
	@echo "$(YELLOW)🚀 Starting Codexion build...$(RESET)"

$(NAME): $(OBJ)
	@echo "\n$(GREEN)🔗 Linking objects...$(RESET)"
	@$(CC) $(OBJ) -fsanitize=thread -fno-PIE -no-pie -o $(NAME)
	@echo "$(GREEN)✅ Build complete: $(NAME)$(RESET)"

%.o: %.c
	$(call progress_bar)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)🧹 Cleaning object files...$(RESET)"
	@rm -f $(OBJ)

fclean: clean
	@echo "$(RED)🗑️ Removing executable...$(RESET)"
	@rm -f $(NAME)

re: fclean all