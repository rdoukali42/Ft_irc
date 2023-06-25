# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/17 00:54:12 by rdoukali          #+#    #+#              #
#    Updated: 2023/06/24 21:10:07 by rdoukali         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRC		=	${shell find . -name "*.cpp"}
HDR		=	${shell find . -name "*.hpp"}

FLAGS = 	-Wall -Wextra -Werror -std=c++98
CC = 	c++
OBJ = 	$(SRC:.cpp=.o)

all: $(NAME)

%.o: %.c
	@c++ -c $^ -o $@ --silent

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "\033[31m	   ༺ (\033[31m\033[32mIRC Compiled!\033[31m)༻\033[39m\n"

clean :
	@echo "\033[33m     Clean  ALL ..."
	@rm -f ${OBJ}
	
fclean : clean
	@rm -f	${NAME}
	@echo "\033[36mEverything is cleaned! \033[32m✓\033[39m\n"

re: fclean all

.PHONY:all clean fclean re