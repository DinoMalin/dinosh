CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g
SHELL = bash 

FILES =	is/ft_isdigit is/ft_isalnum is/ft_isalpha is/ft_isascii is/ft_isprint \
		mem/ft_memset mem/ft_bzero mem/ft_memcpy mem/ft_memmove mem/ft_memchr mem/ft_memcmp mem/ft_calloc \
		str/ft_strlen str/ft_strlcpy str/ft_strlcat str/ft_toupper str/ft_tolower str/ft_strchr str/ft_strrchr str/ft_strncmp str/ft_strnstr str/ft_atoi \
		str/ft_strdup str/ft_strcmp str/ft_substr str/ft_strjoin str/ft_strjoin_free str/ft_strtrim str/ft_split str/ft_itoa str/ft_strmapi str/ft_striteri \
		put/ft_putchar_fd put/ft_putstr_fd put/ft_putendl_fd put/ft_putnbr_fd put/ft_puthexa_fd put/ft_putunbr_fd \
		lst/ft_lstnew lst/ft_lstadd_back lst/ft_lstadd_front lst/ft_lstclear lst/ft_lstdelone lst/ft_lstiter lst/ft_lstmap lst/ft_lstsize lst/ft_lstlast \
		printf/ft_printf printf/ft_fprintf get_next_line/get_next_line_utils get_next_line/get_next_line

OBJ_DIR = obj/
SRCS = $(addprefix src/, $(addsuffix .c, $(FILES)))
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))

NAME = libft.a

.PHONY = all clean fclean re
SH=bash

all: $(OBJS)
	@echo -e "\e[32m✔ Compilating lib source files...\e[37m"
	@ar -rcs $(NAME) $(OBJS)
	@echo -e "\e[32m✔ Library created.\e[37m"

clean:
	@echo -e "\e[31m✔ Deleting lib source files...\e[37m"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

