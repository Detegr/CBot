CC=gcc
FLAGS=-Wall -Wextra -std=c99 -ggdb
LIBS=
EXECUTABLE=a.out

all: main.c connection.o utils.o config.o
	$(CC) -o $(EXECUTABLE) main.c connection.o utils.o config.o $(FLAGS) $(LIBS)
connection.o: connection.c
	$(CC) -c $(FLAGS) connection.c
utils.o: utils.c
	$(CC) -c $(FLAGS) utils.c
config.o: config.c
	$(CC) -c $(FLAGS) config.c
#parser.o: parser.cpp
#	$(CC) -c $(FLAGS) parser.cpp
#urltostring.o: urltostring.c
#	$(CC) -c $(FLAGS) urltostring.c
clean:
	rm *.o bot
configtest: configtest.c config.c
	$(CC) -o configtest configtest.c config.c $(FLAGS) $(LIBS)
