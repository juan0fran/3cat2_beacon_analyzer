#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <socket_utils.h>
#include <time.h>
#include <3cat2_telemetry.h>

int main (int argc, char ** argv) {
	if (argc < 3){
		printf("Usage: ./exec_name tcp_server_ip tcp_server_port\n");
		printf("Example: If listening a GNURadio TCP Server on localhost:\n");
		printf("\t./kiss 127.0.0.1 52001\n");
		exit(-1);
	}
	int i;
	int fd = initialise_client_socket(argv[1], argv[2]);
	printf("Connected to the socket succesfully\n");
	ax25_packet_t ax25;
	char buffer[256];
	int size;
	ErrorHandler err;
	while (1){
		size = read_kiss_from_socket(fd, buffer);
		if (size == NO_FRAME || size == NO_BEACON){
			/* Next iteration */
			continue;
		}
		err = kiss_ax25_unpack(buffer, size, &ax25);
		if (err == NO_ERROR){
			err = decode_3cat2_packet(&ax25);
			free(ax25.info);
		}
		/* NEEDS TO BE FREE'ED */
	}
}


















