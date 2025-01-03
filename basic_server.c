#include "pipe_networking.h"

int main() {
	int to_client;
	int from_client;

	from_client = server_handshake( &to_client );

	int buf;
	while (1) {
		// RNG seeded by handshake
		buf = rand() % 101;
		printf("Sending %d to client(s)\n", buf);
		safe_write(to_client, &buf, sizeof(int), "downstream");
		sleep(1);
	}
}
