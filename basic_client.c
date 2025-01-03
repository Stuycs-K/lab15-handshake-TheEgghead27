#include "pipe_networking.h"

int main() {
	int to_server;
	int from_server;

	from_server = client_handshake( &to_server );

	// do things
	int buf;
	while (1) {
		printf("Client reading value...\n");
		safe_read(from_server, &buf, sizeof(int), "downstream");
		printf("Client read %d (%x)\n", buf, buf);
	}
}
