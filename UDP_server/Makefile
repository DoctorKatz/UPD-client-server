NAME	:=	udp_server

# directories
SRC_DIR	:=	./src
INC_DIR	:=	./inc
OBJ_DIR	:=	./obj

# src / obj files
SRC	:=	main.cpp \
		interaction.cpp \
		lib.cpp \
		udp_server.cpp

OBJ	:=	$(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

# compiler and flags
CC:=	gcc
CFLAGS:=	-std=c++2a -lstdc++ -pipe#-Wall -Wextra -Werror -pedantic
OFLAGS:=	#-pipe

# libraries
#L_FT	:= $(LIB_DIR)/
#include $(L_FT)/libft.mk

.PHONY: all clean fclean re

all:
	mkdir -p $(OBJ_DIR)
	@$(MAKE) $(NAME) --no-print-directory

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS)  -I $(INC_DIR) -o $@ -c $<

$(NAME): $(OBJ)
	$(CC) $(OFLAGS) $(OBJ) -o $(NAME)

run:
	mkdir -p $(OBJ_DIR)
	@$(MAKE) $(NAME) --no-print-directory

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS)  -I $(INC_DIR) -o $@ -c $<

$(NAME): $(OBJ)
	$(CC) $(OFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re:
	@$(MAKE) fclean --no-print-directory
	@$(MAKE) all --no-print-directory
