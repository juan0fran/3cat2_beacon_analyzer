#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <socket_utils.h>
#include <time.h>
#include <util.h>
#include <3cat2_telemetry.h>

int main (int argc, char ** argv) {
	int user_flag;
	if (argc < 3){
		printf("Usage: ./exec_name tcp_server_ip tcp_server_port user_flag\n");
		printf("Example: If listening a GNURadio TCP Server on localhost:\n");
		printf("\t./kiss 127.0.0.1 52001\n");
		printf("\tUser flag uses for a RAW output of the data (to put into a DB or Excel file):\n");
		printf("\t    0 for RAW output, 1 for formatted output\n");
		exit(-1);
	}
	int i;
	if (argc == 4)
		user_flag = atoi(argv[3]);
	else
		user_flag = 1;
	initiate_printf(user_flag);
	int fd = initialise_client_socket(argv[1], argv[2]);
	do_printf("Connected to the socket succesfully\n");
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
			if (user_flag == 0){
				err = rawprint_3cat2_packet(&ax25);
				free(ax25.info);		
			}else{		
				err = decode_3cat2_packet(&ax25);
				free(ax25.info);
			}
		}
		/* NEEDS TO BE FREE'ED */
	}
}


















