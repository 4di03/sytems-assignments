all : main test 

main : main.c encrypter.c randomStringGen.c queue.c
	gcc -o main main.c encrypter.c randomStringGen.c queue.c

test : tests.c encrypter.c randomStringGen.c queue.c
	gcc -o test tests.c encrypter.c randomStringGen.c queue.c
