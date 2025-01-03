#include <signal.h>
#include "pipe_networking.h"

static int to_client;
static int from_client;

static void handler(int sig) {
	switch(sig) {
		case SIGPIPE:
			printf("Client disconnected\n");
			break;
		case SIGQUIT:
			remove(WKP);
			close(to_client);
			close(from_client);
			break;
		default:
			printf("Unknown signal %d\n", sig);
	}
}

int main() {
	int buf;
	signal(SIGPIPE, handler);
	signal(SIGQUIT, handler);
	while (1) {
		from_client = server_handshake( &to_client );
		printf("shooketh\n");
		while (1) {
			// RNG seeded by handshake
			buf = rand() % 101;
			printf("Sending %d to client(s)\n", buf);
			if (write(to_client, &buf, sizeof(int)) == -1)
				break;
			sleep(1);
		}
		close(to_client);
		close(from_client);
	}
}
