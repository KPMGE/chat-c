CC = gcc

all: 
	$(CC) -o server server.c

clean:
	rm server
