all: server client tests

server: server.c src/server_commands.c src/constants.h src/utils.c 
	gcc server.c src/server_commands.c src/constants.h src/utils.c -o exec/server 

client: client.c src/client_commands.c src/constants.h src/utils.c 
	gcc client.c src/client_commands.c src/constants.h src/utils.c -o rfs

tests: tests.c src/server_commands.c src/client_commands.c src/constants.h src/utils.c
	gcc tests.c src/server_commands.c src/client_commands.c src/constants.h src/utils.c -o exec/tests