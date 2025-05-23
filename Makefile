# Program name
NAME = ft_ping

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror
DEBUG_FLAGS = -g3 -fsanitize=address
RM = rm -f

# Source files
SRCS = src/main.c \
       src/ping.c \
       src/socket.c \
       src/packet.c \
       src/utils.c

# Object files and includes
OBJS = $(SRCS:.c=.o)
INCLUDES = -I./includes

# Installation paths
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

# Colors and formatting
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
RED = \033[0;31m
BOLD = \033[1m
RESET = \033[0m

# Progress indicators
CHECK = $(GREEN)✓$(RESET)
CROSS = $(RED)✗$(RESET)
ARROW = $(BLUE)➜$(RESET)

# Default target
all: $(NAME)

# Main compilation
$(NAME): $(OBJS)
	@echo "$(BOLD)$(ARROW) Compiling $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
	@echo "$(CHECK) $(GREEN)$(NAME) successfully compiled!$(RESET)"

# Object file compilation
%.o: %.c
	@echo "$(ARROW) Compiling $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: fclean $(NAME)
	@echo "$(CHECK) $(GREEN)Debug build created!$(RESET)"

# Clean object files
clean:
	@echo "$(ARROW) Cleaning object files..."
	@$(RM) $(OBJS)
	@echo "$(CHECK) $(GREEN)Clean complete!$(RESET)"

# Full clean
fclean: clean
	@echo "$(ARROW) Removing $(NAME)..."
	@$(RM) $(NAME)
	@echo "$(CHECK) $(GREEN)Full clean complete!$(RESET)"

# Install program
install: $(NAME)
	@echo "$(ARROW) Installing $(NAME) to $(BINDIR)..."
	@sudo mkdir -p $(BINDIR)
	@sudo cp $(NAME) $(BINDIR)/
	@echo "$(CHECK) $(GREEN)$(NAME) successfully installed!$(RESET)"

# Uninstall program
uninstall:
	@echo "$(ARROW) Uninstalling $(NAME)..."
	@sudo $(RM) $(BINDIR)/$(NAME)
	@echo "$(CHECK) $(GREEN)$(NAME) successfully uninstalled!$(RESET)"

# Rebuild everything
re: fclean all

# Help target
help:
	@echo "$(BOLD)Available targets:$(RESET)"
	@echo "  $(GREEN)all$(RESET)        - Build $(NAME)"
	@echo "  $(GREEN)debug$(RESET)      - Build $(NAME) with debug flags"
	@echo "  $(GREEN)clean$(RESET)      - Remove object files"
	@echo "  $(GREEN)fclean$(RESET)     - Remove object files and $(NAME)"
	@echo "  $(GREEN)re$(RESET)         - Rebuild everything"
	@echo "  $(GREEN)install$(RESET)    - Install $(NAME) to $(BINDIR)"
	@echo "  $(GREEN)uninstall$(RESET)  - Remove $(NAME) from $(BINDIR)"
	@echo "  $(GREEN)help$(RESET)       - Show this help message"

.PHONY: all clean fclean re install uninstall debug help
