#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "pipe_networking.h"

#define DEBUG

void fatal(const char * msg) {
	if (errno)
		perror(msg);
	exit(errno);
}
void clean_fatal(const char * msg, const char * fname) {
	remove(fname);
	fatal(msg);
}

int safe_read(int fd, void *buf, size_t count, const char *name) {
	int ret;
	if ((ret = read(fd, buf, count)) <= 0)
		fatal(name);
	return ret;
}
int safe_write(int fd, void *buf, size_t count, const char *name) {
	int ret;
	if ((ret = write(fd, buf, count)) <= 0)
		fatal(name);
	return ret;
}

//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
	server_setup

	creates the WKP and opens it, waiting for a connection.
	removes the WKP once a connection has been made

	returns the file descriptor for the upstream pipe.
	=========================*/
int server_setup() {
	srand(time(NULL));
#ifdef DEBUG
	printf("Server making the pipe `"WKP"`.\n");
#endif
	if (mkfifo(WKP, 0666))
		fatal(WKP);
#ifdef DEBUG
	printf("Server opening `"WKP"`.\n");
#endif
	int from_client = open(WKP, O_RDONLY);
	if (from_client < 0)
		fatal(WKP);
#ifdef DEBUG
	printf("Server removing `"WKP"`\n");
#endif
	remove(WKP);
	return from_client;
}

/*=========================
	server_handshake
	args: int * to_client

	Performs the server side pipe 3 way handshake.
	Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

	returns the file descriptor for the upstream pipe (see server setup).
	=========================*/
int server_handshake(int *to_client) {
	int from_client = server_setup();
	return server_handshake_half(from_client, to_client);
}
int server_handshake_half(int from_client, int *to_client) {
#ifdef DEBUG
	printf("Server reading SYN PID\n");
#endif
	safe_read(from_client, to_client, sizeof(int), WKP);
	char fname[16];  // 10 for int max, + 5 for .fifo + 1
	sprintf(fname, "%d.fifo", *to_client);
#ifdef DEBUG
	printf("Server opening `%s`\n", fname);
#endif
	*to_client = open(fname, O_WRONLY);
	if (*to_client < 0)
		fatal(fname);

	int buf[2];
	buf[0] = rand();
#ifdef DEBUG
	printf("Server sending SYN_ACK value %d\n", buf[0]);
#endif
	safe_write(*to_client, buf, sizeof(int), fname);

#ifdef DEBUG
	printf("Server awaiting ACK\n");
#endif
	safe_read(from_client, buf+1, sizeof(int), WKP);
	if (buf[0]+1 != buf[1]) {
		errno = EKEYREJECTED;
		fatal("ACK FAIL!!!");
	}
#ifdef DEBUG
	printf("Server got correct ACK, handshake complete\n");
#endif
	return from_client;
}


/*=========================
	client_handshake
	args: int * to_server

	Performs the client side pipe 3 way handshake.
	Sets *to_server to the file descriptor for the upstream pipe.

	returns the file descriptor for the downstream pipe.
	=========================*/
int client_handshake(int *to_server) {
	int from_server;

	int pid = getpid();
	char fname[16];  // 10 for int max, + 5 for .fifo + 1
	sprintf(fname, "%d.fifo", pid);
#ifdef DEBUG
	printf("Client private pipe `%s`\n", fname);
#endif
	if (mkfifo(fname, 0666))
		fatal("mkfifo");

#ifdef DEBUG
	printf("Client opening `"WKP"`\n");
#endif
	*to_server = open(WKP, O_WRONLY);
	if (*to_server < 0)
		clean_fatal(WKP, fname);
#ifdef DEBUG
	printf("Client creating fifo PID %s\n", fname);
#endif
	if (write(*to_server, &pid, sizeof(int)) < 0)
		clean_fatal(WKP, fname);

#ifdef DEBUG
	printf("Client opening %s for SYN_ACK\n", fname);
#endif
	from_server = open(fname, O_RDONLY);
	if (from_server < 0)
		clean_fatal(fname, fname);

#ifdef DEBUG
	printf("Client deleting `%s`\n", fname);
#endif
	remove(fname);

	int buf;
#ifdef DEBUG
	printf("Client reading SYN_ACK value\n");
#endif
	safe_read(from_server, &buf, sizeof(int), fname);
	buf++;

#ifdef DEBUG
	printf("Client sending ACK value %d\n", buf);
#endif
	safe_write(*to_server, &buf, sizeof(int), WKP);
	return from_server;
}


/*=========================
	server_connect
	args: int from_client

	handles the subserver portion of the 3 way handshake

	returns the file descriptor for the downstream pipe.
	=========================
int server_connect(int from_client) {
	int to_client = 0;
	return to_client;
}
*/
