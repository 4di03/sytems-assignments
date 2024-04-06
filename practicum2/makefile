all: server client tests

server: server.c commands.c
	gcc server.c commands.c -o server 

client: client.c commands.c
	gcc client.c commands.c -o client

tests: tests.c commands.c
	gcc tests.c commands.c -o test