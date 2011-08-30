CC=gcc
FLAGS=-Wall -Wextra -std=c99 -ggdb -DDEBUG -DCBOT_PYTHON -I/usr/include/python2.6/
LIBS=-lpython2.6
EXECUTABLE=a.out

all: main.c connection.o utils.o config.o bot.o module.o
	$(CC) -o $(EXECUTABLE) main.c connection.o bot.o utils.o config.o module.o $(FLAGS) $(LIBS)
connection.o: connection.c
	$(CC) -c $(FLAGS) connection.c
bot.o: bot.c
	$(CC) -c $(FLAGS) bot.c
utils.o: utils.c
	$(CC) -c $(FLAGS) utils.c
config.o: config.c
	$(CC) -c $(FLAGS) config.c
module.o: module.c
	$(CC) -c $(FLAGS) module.c
#parser.o: parser.cpp
#	$(CC) -c $(FLAGS) parser.cpp
#urltostring.o: urltostring.c
#	$(CC) -c $(FLAGS) urltostring.c
clean:
	rm *.o bot
configtest: configtest.c config.c
	$(CC) -o configtest configtest.c config.c $(FLAGS) $(LIBS)
