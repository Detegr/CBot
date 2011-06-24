CC=gcc
FLAGS=-Wall -Wextra -std=c99 -ggdb
LIBS=
EXECUTABLE=a.out

all: main.c connection.o utils.o
	$(CC) -o $(EXECUTABLE) main.c connection.o utils.o $(FLAGS) $(LIBS)
connection.o: connection.c
	$(CC) -c $(FLAGS) connection.c
utils.o: utils.c
	$(CC) -c $(FLAGS) utils.c
#parser.o: parser.cpp
#	$(CC) -c $(FLAGS) parser.cpp
#urltostring.o: urltostring.c
#	$(CC) -c $(FLAGS) urltostring.c
clean:
	rm *.o bot
check-syntax:
	gcc -o nul -Wall -S ${CHK_SOURCES}
