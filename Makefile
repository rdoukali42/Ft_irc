# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rdoukali <rdoukali@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/17 00:54:12 by rdoukali          #+#    #+#              #
#    Updated: 2023/06/18 19:17:43 by rdoukali         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRC		=	${shell find . -name "*.cpp"}
HDR		=	${shell find . -name "*.hpp"}

FLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++
OBJ = 	$(SRC:.cpp=.o)

all: $(NAME)

%.o: %.c
	@c++ -c $^ -o $@

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "\033[31m	   ༺ (\033[31m\033[32mIRC Compiled!\033[31m)༻\033[39m\n"

# CC = c++
# CFLAGS = -Wall -Werror -Wextra -std=c++98

# all : ${NAME}

# ${NAME} : ${SRC}
# 	@${CC} ${CFLAGS} -o ${NAME} ${SRC}
# 	@echo "\033[31m	   ༺ (\033[31m\033[32mIRC Compiled!\033[31m)༻\n"

clean :
	@echo "\033[33m     Clean  ALL ..."
	@rm -f ${OBJ}
	
fclean : clean
	@rm -f	${NAME}
	@echo "\033[36mEverything is cleaned! \033[32m✓\n"

re: fclean all

.PHONY:all clean fclean re