NAME = webserv

CFLAGS = -Wall -Wextra -Werror

CC = clang++

FILE = \
BaseException \
ConfigLocation \
ConfigServer \
ConfigServers \
ConstatsParametrs \
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
# $(addsuffix .cpp,$(TOOLS))

BINS = $(addsuffix .o,$(FILE)) \
$(addsuffix .o,$(TOOLS))


all: $(NAME) $(HEADERS) $(SRCS) 

$(NAME): $(BINS)
	$(CC) $(CFLAGS) $(BINS) -o $(NAME)
	@echo "\033[1;32m"
	@echo "$(NAME) created"
	@echo "\033[0m"

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BINS)
	@echo "\033[1;33mclean completed\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[1;31mfclean completed\033[0m"

re: fclean all

.PHONY: all clean fclean re