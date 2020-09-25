all:
	gcc Server.c -o server.o
	gcc client.c -o client.o
clean:
	rm server.o client.o
