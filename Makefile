NAME = webserv

CFLAGS = #-Wall -Wextra -Werror

CC = clang++

FILE = \
BaseException \
CGI \
ConfigLocation \
ConfigServer \
ConfigServers \
ConstantParametrs \
ConstatsParametrs \
Node \
Request \
Response \
Server


TOOLS = \
tools \
webserv

HEADERS = $(addsuffix .hpp,$(FILE)) \
color.hpp \
headers.hpp

SRCS = $(addsuffix .cpp,$(FILE)) \
$(addsuffix .cpp,$(TOOLS))

BIN_DIR = Obj/
BINS_FILE = $(addsuffix .o,$(FILE)) \
$(addsuffix .o,$(TOOLS))
BINS = $(addprefix $(BIN_DIR), $(BINS_FILE))
BINS_TEMP = $(addprefix $(BIN_DIR), %.o)

all: $(BIN_DIR) $(NAME) $(HEADERS) $(SRCS) 

$(NAME): $(BINS)
	@$(CC) $(CFLAGS) $(BINS) -o $(NAME)
	@echo "\033[1;32m$(NAME) created\033[0m"

$(BINS_TEMP): %.cpp $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	@mkdir $(BIN_DIR)

clean:
	@rm -rf $(BIN_DIR)
	@echo "\033[1;33mclean completed\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[1;31mfclean completed\033[0m"

re: fclean all

.PHONY: all clean fclean re
