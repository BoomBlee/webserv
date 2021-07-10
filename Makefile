# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cmalt <cmalt@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/19 07:17:49 by cmalt             #+#    #+#              #
#    Updated: 2021/01/19 10:59:06 by cmalt            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =	a.out

CLANG =	clang++

SRC =	BaseException.cpp \
		CGI.cpp \
		ConstantParametrs.cpp \
		Node.cpp \
		Request.cpp \
		Response.cpp \
		Server.cpp \
		tools.cpp \
		webserv.cpp

OBJ =	$(SRC:.cpp=.o)

HEAD =	.

all: $(NAME)

%.o: %.cpp
	$(CLANG) -I $(HEAD) -c $< -o $@

$(NAME): $(OBJ)
	$(CLANG) $^ -g

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: re clean fclean all