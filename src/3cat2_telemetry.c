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

static void PrintBeacon(char * message){
	int first_beacon_part[7];
	float second_beacon_part[6];
	sscanf(message, "%d %d %d %d %d\t%d %d %e %e %e %e %e %e", 
				first_beacon_part, first_beacon_part+1, first_beacon_part+2, first_beacon_part+3,
				first_beacon_part+4, first_beacon_part+5, first_beacon_part+6,
				second_beacon_part, second_beacon_part+1, second_beacon_part+2,
				second_beacon_part+3, second_beacon_part+4, second_beacon_part+5);
	if ( (*first_beacon_part+5) == 0 ){
		/* Whe are in DETUMBLING */
			printf("Beacon Received...:\n\tMode: %d. Battery (mV): %d. Current Consumption (mA): %d. EPS Temp: %d. Antenna Temp: %d.\n"
			"\tADCS: Detumbling. Control Flag: %d. Deriv_Mag (nT): %e X %e Y %e Z. VoltageControl: %e X %e Y %e Z\n\n", 
			first_beacon_part[0], 
			first_beacon_part[1], first_beacon_part[2], first_beacon_part[3], first_beacon_part[4], 
			first_beacon_part[6],
			second_beacon_part[0], second_beacon_part[1], second_beacon_part[2],
			second_beacon_part[3], second_beacon_part[4], second_beacon_part[5]);
	}else{
			printf("Beacon Received...:\n\tMode: %d. Battery (mV): %d. Current Consumption (mA): %d. EPS Temp: %d. Antenna Temp: %d.\n"
			"\tADCS: SS-Nominal. Control Flag: %d. Sun_Vector: %e X %e Y %e Z. VoltageControl: %e X %e Y %e Z\n\n", 
			first_beacon_part[0], 
			first_beacon_part[1], first_beacon_part[2], first_beacon_part[3], first_beacon_part[4], 
			first_beacon_part[6],
			second_beacon_part[0], second_beacon_part[1], second_beacon_part[2],
			second_beacon_part[3], second_beacon_part[4], second_beacon_part[5]);
	}
}

ErrorHandler rawprint_3cat2_packet(ax25_packet_t * in){
	printf("%s\n", &in->info[1]);
	return NO_ERROR;
}

ErrorHandler decode_3cat2_packet(ax25_packet_t * in){
	if ((unsigned char) in->info[0] == 0xFF){
		PrintBeacon(in->info+1);
		return NO_ERROR;
	}else{
		printf("Not a beacon message\n");
		return NO_BEACON;
	}
}

int read_kiss_from_socket(int fd, char * buffer){
	int i;
	int out_size;
	int size;
	char byte;
	bool no_frame;
	bool transpose;
	byte = 0x00;
	/* First of all read the two first bytes */
	while((unsigned char)byte != FEND){
		size = read(fd, &byte, 1);
		if (size <= 0){
			if (size == 0){
				do_printf("End of socket\n");
			}else{
				perror("Error reading socket -> ");
			}
			exit(-1);
		}
	}
	if ((unsigned char)byte == FEND){
		size = read(fd, &byte, 1);
		if (size <= 0){
			if (size == 0){
				do_printf("End of socket\n");
			}else{
				perror("Error reading socket -> ");
			}
			exit(-1);
		}
		if ((unsigned char)byte == 0x00){
			do_printf("Frame found:\n");
		}else{
			do_printf("Control frame found: ");
			no_frame = true;
			i = 0;
			transpose = false;
			while(no_frame){
				size = read(fd, &byte, 1);
				if (size <= 0){
					if (size == 0){
						do_printf("End of socket\n");
					}else{
						perror("Error reading socket -> ");
					}
					exit(-1);
				}
				if ((unsigned char)byte == FEND){
					no_frame = false;
				}else if (transpose == true){
					if ((unsigned char) byte == TFEND){
						buffer[i] = FEND;
					}else if ((unsigned char) byte == TFESC){
						buffer[i] = FESC;
					}
					transpose = false;
					i++;
				}else if ((unsigned char) byte == FESC){
					transpose = true;
				}else{
					buffer[i] = byte;
					i++;
				}
			}
			buffer[i] = '\0';
			do_printf("%s\n", buffer);
			return NO_BEACON;
		}
	}else{
		return NO_FRAME;
	}
	no_frame = true;
	i = 0;
	transpose = false;
	while(no_frame){
		size = read(fd, &byte, 1);
		if (size <= 0){
			if (size == 0){
				do_printf("End of socket\n");
			}else{
				perror("Error reading socket -> ");
			}
			exit(-1);
		}
		if ((unsigned char)byte == FEND){
			no_frame = false;
		}else if (transpose == true){
			if ((unsigned char) byte == TFEND){
				buffer[i] = FEND;
			}else if ((unsigned char) byte == TFESC){
				buffer[i] = FESC;
			}
			transpose = false;
			i++;
		}else if ((unsigned char) byte == FESC){
			transpose = true;
		}else{
			buffer[i] = byte;
			i++;
		}
	}
	return i;
}

ErrorHandler kiss_ax25_unpack(char * buffer, int size, ax25_packet_t * out){
	int i, j;
	out->size = size;

	if (out->size < 17){
		do_printf("Frame discarded, too short packet\n");
		return NO_FRAME;
	}

	for (i = 0; i < 14; i++){
		buffer[i] = (buffer[i]&0xFF) >> 1;
	}
	if (buffer[6] == '\0' && buffer[13] == '\0'){
		strcpy(out->to, buffer);
		strcpy(out->from, buffer+7);
	
		do_printf("\tFrom: %s\n", out->from);
		do_printf("\tTo: %s\n", out->to);

	}else{
		do_printf("Frame discarded, wrong addresses (not following ax25 format)\n");
		return NO_FRAME;
	}
	out->control = *buffer+14;
	out->pid = *buffer+15;

	out->info = malloc(out->size - 16);
	memcpy(out->info, buffer+16, out->size - 16);

	return NO_ERROR;

}