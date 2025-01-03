compile all: client.exe server.exe
client: client.exe
	@./client.exe
server: server.exe
	@./server.exe
client.exe: basic_client.o pipe_networking.o
	@gcc -o client.exe basic_client.o pipe_networking.o

server.exe: basic_server.o pipe_networking.o
	@gcc -o server.exe basic_server.o pipe_networking.o

basic_client.o: basic_client.c pipe_networking.h
	@gcc -c basic_client.c

basic_server.o: basic_server.c pipe_networking.h
	@gcc -c basic_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	@gcc -c pipe_networking.c

clean:
	@rm -f *.o client server *~
