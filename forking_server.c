#include <signal.h>
#include "pipe_networking.h"

static int to_client;
static int from_client;

static void handler(int sig) {
	switch(sig) {
		case SIGPIPE:
			printf("Client disconnected\n");
			break;
		case SIGINT:
			printf("Interrupted, cleaning up...\n");
			remove(WKP);
			close(to_client);
			close(from_client);
			printf("Exiting...\n");
			exit(0);
		default:
			printf("Unknown signal %d\n", sig);
	}
}

int main() {
	int buf;
	signal(SIGPIPE, handler);
	signal(SIGINT, handler);
	while (1) {
		from_client = server_setup();
		switch(fork()) {
			case 0:
				server_handshake_half( from_client, &to_client );
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
				return 0;
			case -1:
				perror("fork");
				return errno;
			default:
				break;
		}
	}
}
