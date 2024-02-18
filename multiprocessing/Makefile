all : main test tf

main : main.c src/encrypter.c src/randomStringGen.c src/queue.c src/mp.c src/program.c
	gcc -o main main.c src/encrypter.c src/randomStringGen.c src/queue.c src/polybius.c src/mp.c src/program.c -lpthread 

test : src/tests.c src/encrypter.c src/randomStringGen.c src/queue.c src/polybius.c src/queue.c src/mp.c  src/program.c 
	gcc -o test src/tests.c src/encrypter.c src/randomStringGen.c src/polybius.c src/queue.c src/mp.c src/program.c -lpthread 

tf : src/test_fork.c src/queue.c
	gcc -o test_fork  src/test_fork.c src/queue.c -lpthread