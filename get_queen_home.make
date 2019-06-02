GET_QUEEN_HOME_C_FLAGS=-O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

get_queen_home: get_queen_home.o
	gcc -o get_queen_home get_queen_home.o

get_queen_home.o: get_queen_home.c get_queen_home.make
	gcc -c ${GET_QUEEN_HOME_C_FLAGS} -o get_queen_home.o get_queen_home.c

clean:
	rm -f get_queen_home get_queen_home.o
