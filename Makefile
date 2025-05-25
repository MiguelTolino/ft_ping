# Program name
NAME = ft_ping

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror
DEBUG_FLAGS = -g3 -fsanitize=address
RM = rm -f

# Source files
SRCS = $(wildcard srcs/*.c) \
       $(wildcard srcs/network/*.c) \
       $(wildcard srcs/utils/*.c)

# Build directory
BUILD_DIR = build
OBJS_DIR = $(BUILD_DIR)/obj

# Object files and includes
OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)
INCLUDES = -I includes

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
all: $(BUILD_DIR)/$(NAME)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(OBJS_DIR)/srcs
	@mkdir -p $(OBJS_DIR)/srcs/network
	@mkdir -p $(OBJS_DIR)/srcs/utils

# Main compilation
$(BUILD_DIR)/$(NAME): $(OBJS)
	@echo "$(BOLD)$(ARROW) Compiling $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS)
	@echo "$(CHECK) $(GREEN)$(NAME) successfully compiled!$(RESET)"

# Object file compilation
$(OBJS_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "$(ARROW) Compiling $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: fclean $(BUILD_DIR)/$(NAME)
	@echo "$(CHECK) $(GREEN)Debug build created!$(RESET)"

# Clean object files
clean:
	@echo "$(ARROW) Cleaning object files..."
	@$(RM) -r $(BUILD_DIR)
	@echo "$(CHECK) $(GREEN)Clean complete!$(RESET)"

# Full clean
fclean: clean
	@echo "$(CHECK) $(GREEN)Full clean complete!$(RESET)"

# Install program
install: $(BUILD_DIR)/$(NAME)
	@echo "$(ARROW) Installing $(NAME) to $(BINDIR)..."
	@sudo mkdir -p $(BINDIR)
	@sudo cp $(BUILD_DIR)/$(NAME) $(BINDIR)/
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
	@echo "  $(GREEN)clean$(RESET)      - Remove build directory"
	@echo "  $(GREEN)fclean$(RESET)     - Remove build directory"
	@echo "  $(GREEN)re$(RESET)         - Rebuild everything"
	@echo "  $(GREEN)install$(RESET)    - Install $(NAME) to $(BINDIR)"
	@echo "  $(GREEN)uninstall$(RESET)  - Remove $(NAME) from $(BINDIR)"
	@echo "  $(GREEN)help$(RESET)       - Show this help message"

.PHONY: all clean fclean re install uninstall debug help
