NAME = ft_ping
CC = gcc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f
SRCS = src/main.c \
       src/ping.c \
       src/socket.c \
       src/packet.c \
       src/utils.c

OBJS = $(SRCS:.c=.o)
INCLUDES = -I./includes

# Colors for terminal output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(YELLOW)Compiling $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) successfully compiled!$(RESET)"

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(YELLOW)Removing object files...$(RESET)"
	@$(RM) $(OBJS)

fclean: clean
	@echo "$(YELLOW)Removing $(NAME)...$(RESET)"
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
