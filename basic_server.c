#include "pipe_networking.h"

int main() {
	int to_client;
	int from_client;

	from_client = server_handshake( &to_client );

	int buf;
	printf("Server reading magic value...\n");
	safe_read(from_client, &buf, sizeof(int), "upstream");
	buf--;
	printf("Server sending decremented magic value %d %x\n", buf, buf);
	safe_write(to_client, &buf, sizeof(int), "downstream");
	printf("Server sent\n");
}
