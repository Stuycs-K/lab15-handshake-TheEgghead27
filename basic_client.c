#include "pipe_networking.h"

int main() {
	int to_server;
	int from_server;

	from_server = client_handshake( &to_server );

	// do things
	int magic = 123456, buf;
	printf("Client sending magic value %d\n", magic);
	safe_write(to_server, &magic, sizeof(int), "upstream");
	printf("Client reading magic value...\n");
	safe_read(from_server, &buf, sizeof(int), "downstream");
	printf("Client read %d %x\n", buf, buf);
}
