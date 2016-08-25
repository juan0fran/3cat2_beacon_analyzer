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
#include <util.h>

ErrorHandler read_from_file(FILE * fp, ax25_packet_t * out){
	char buffer[256];
	char numberArray[256];
	int consumed;
	int accumulated;
	int i;
	if (fgets(buffer, sizeof(buffer), fp) != NULL){
		consumed = 0;
		accumulated = 0;
		if ((unsigned char)buffer[0] != 0x20){
			return NO_FRAME;
		}
		i = 0;
		while (accumulated < strlen(buffer)){
			sscanf(buffer+accumulated, " %2x%n", (int *)&numberArray[i], &consumed);
			i++;
			accumulated += consumed;
		}
		out->info = malloc(i);
		out->size = i;
		memcpy(out->info, numberArray, i);
		return NO_ERROR;
	}else{
		exit(-1);
	}
}

int main (int argc, char ** argv) {
	if (argc < 2){
		printf("Usage: ./exec_name file_name user_flag\n");
		printf("User flag uses for a RAW output of the data (to put into a DB or Excel file)\n");
		printf("\t0 for RAW output, 1 for formatted output\n");
		exit(-1);
	}
	FILE * fp;
	int i;
	int user_flag;
	if (argc == 3)
		user_flag = atoi(argv[2]);
	else
		user_flag = 1;
	initiate_printf(user_flag);
	ax25_packet_t ax25;
	ErrorHandler err;
	fp = fopen(argv[1], "r+");
	if (fp == NULL){
		do_printf("Could not open the file\n");
		exit(-1);
	}
	while (1){
		err = read_from_file(fp, &ax25);
		/* NEEDS TO BE FREE'ED */
		if (err == NO_ERROR){
			if (user_flag == 0){
				err = rawprint_3cat2_packet(&ax25);
				free(ax25.info);		
			}else{		
				err = decode_3cat2_packet(&ax25);
				free(ax25.info);
			}
		}
	}
}
