main: main.c
	gcc main.c -o main

main-debug: main.c
	gcc -g main.c -o main-debug
