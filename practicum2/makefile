all: server client tests

server: src/server.c src/server_commands.c src/constants.h src/utils.c 
	gcc src/server.c src/server_commands.c src/constants.h src/utils.c -o exec/server -lpthread 

client: src/client.c src/client_commands.c src/constants.h src/utils.c 
	gcc src/client.c src/client_commands.c src/constants.h src/utils.c -o rfs -lpthread 

tests: src/tests.c src/server_commands.c src/client_commands.c src/constants.h src/utils.c 
	gcc src/tests.c src/server_commands.c src/client_commands.c src/constants.h src/utils.c -o exec/tests -lpthread 