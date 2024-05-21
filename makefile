GPC=g++
FLAGS=-g
NAME=main
OBJ=lex.o syn.o ident_tables.o connection.o poliz_types.o main.o

%.o: %.cpp
	$(GPC) -c -o $@ $< $(FLAGS)

$(NAME): $(OBJ)
	$(GPC) -o $@ $^ $(FLAGS)
	rm -f $(OBJ)

clean:
	rm -f $(NAME) $(OBJ)
