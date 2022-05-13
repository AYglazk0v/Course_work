################################################

DIRE	=	~/Steps2Hell
CC		=	gcc
FLAGS	=	-Wall -Wextra -Werror -lssl -lcrypto
INCL	=	./includes/
CFLAGS	=	-I $(INCL) $(FLAGS) 

################################################

NAME = steps2hell

################################################

PATH_SRC = src/
PATH_UTILS = src/utils/
PATH_GNL = src/GNL/

################################################

SRC		=	main.c\
			funcRecvRespList.c\
			timingModes.c\
			serv.c

GNL		=	get_next_line_bonus.c\
			get_next_line_utils_bonus.c

UTILS	=	utils_0.c\
			modification_time.c\
			checkStartDir.c\
			listOfFiles.c\
			split.c

################################################

SRC_SH	=	$(addprefix $(PATH_SRC), $(SRC))\
			$(addprefix $(PATH_UTILS), $(UTILS))\
			$(addprefix $(PATH_GNL), $(GNL))


OBJ_SH	=	$(SRC_SH:.c=.o)

################################################

.PHONY: all re clean fclean
.ONESHELL:

################################################

all : $(NAME)
	@if ! [ -d $(DIRE) ] && ! [ -f $(DIRE) ] ; then
		mkdir $(DIRE)
	fi

$(NAME) : $(OBJ_SH)
	$(CC) $(CFLAGS) $(OBJ_SH) -o $(NAME)

clean:
	@rm -rf $(OBJ_SH)

fclean: clean
	@rm -rf $(NAME)

re: fclean all