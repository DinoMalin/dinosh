NAME = dinosh

LIBFT = lib/libft.a

CC = cc

CFLAGS =	-Wall -Wextra -Werror -g \
			-Iinclude

LFLAGS =	-Llib -lft -lreadline

SRC_DIR = src/
OBJ_DIR = obj/
MAIN	= main/main
FILES	= main/errors main/utils main/signals main/vi_mode					\
		utils/memory utils/join	utils/utils									\
		parsing/tokenize parsing/parse parsing/check parsing/merge			\
		parsing/history														\
		builtins/choose builtins/echo builtins/cd							\
		builtins/type builtins/pwd builtins/export							\
		builtins/set builtins/jobs builtins/unset							\
		builtins/env builtins/exit builtins/fg								\
		builtins/bg builtins/hash builtins/test								\
		env/utils env/accessors												\
		jobs/jobs															\
		expanding/vars expanding/file_globing expanding/parameter			\
		expanding/arithmetic expanding/expand expanding/tilde				\
		expanding/param_globing expanding/utils expanding/escape			\
		expanding/control_substitution expanding/process_substitution		\
		expanding/condition													\
		executing/fork executing/path executing/redirect executing/init		\

FUZZER_FILES := $(FILES) utils/fuzzer
FILES += $(MAIN) main/autocomplete

SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(FILES)))
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))


FUZZER_SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(FUZZER_FILES)))
FUZZER_OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FUZZER_FILES)))

TESTS_SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(TESTS_FILES)))
TESTS_OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(TESTS_FILES)))

SHELL=bash

all: $(NAME)

bonus: all

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)
	@make fclean -C lib --silent

re: fclean all

$(NAME): $(OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating source files...\e[37m"
	@$(CC) -o $@ $(OBJS) $(LFLAGS)
	@echo -e "\e[32m✔ Executable created.\e[37m"

fuzzer: CFLAGS += -fsanitize=address,fuzzer -D FUZZER
fuzzer: $(FUZZER_OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating source files...\e[37m"
	@clang -o $@ $(FUZZER_OBJS) $(LFLAGS) $(CFLAGS)
	@echo -e "\e[32m✔ Fuzzer created.\e[37m"

$(LIBFT):
	@make -C lib --silent

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean fclean re bonus
