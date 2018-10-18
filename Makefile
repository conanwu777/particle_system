NAME = test
SRC = *.c
OBJ = $(SRC:.c=.o)
FRAMEWORKS = -framework OpenGl -w
BREW_INC = -I ~/.brew/include
GLFW_LINK = -L ~/.brew/lib -lglfw
CL = -framework OpenCL

all: $(NAME)

$(NAME):
	@echo $(YELLO)Making particle_system
	@gcc $(SRC) -o $(NAME) $(FRAMEWORKS) $(CL) $(BREW_INC) $(GLFW_LINK)
	@echo $(GREEN)Done!
clean:
	@echo $(YELLO)Cleaning o files
	@/bin/rm -f $(OBJ)

fclean: clean
	@echo $(YELLO)Removing excutable
	@rm $(NAME)

re:	fclean all