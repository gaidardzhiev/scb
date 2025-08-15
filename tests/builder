CC = gcc
CFLAGS = -Wall -g
hello: main.o hello.o
    $(CC) $(CFLAGS) main.o hello.o -o hello
main.o: main.c hello.h
    $(CC) $(CFLAGS) -c main.c -o main.o
hello.o: hello.c hello.h
    $(CC) $(CFLAGS) -c hello.c -o hello.o
clean:
    rm -f hello main.o hello.o
