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

#define YES 1
#define NO  0
 
int hexToInt(char s[]) {
    int hexdigit, i, inhex, n;    
    i=0;
     
    if(s[i] == '0') {
        ++i;
        if(s[i] == 'x' || s[i] == 'X'){            
            ++i;
        }
    }
     
    n = 0;
    inhex = YES;
    for(; inhex == YES; ++i) {
        if(s[i] >= '0' && s[i] <= '9') {            
            hexdigit = s[i] - '0';
        } else if(s[i] >= 'a' && s[i] <= 'f') {            
            hexdigit = s[i] - 'a' + 10;
        } else if(s[i] >= 'A' && s[i] <= 'F') {
            hexdigit = s[i] - 'A' + 10;
        } else {
            inhex = NO;
        }
        if(inhex == YES) {
            n = 16 * n + hexdigit;
        }
    }
     
    return n;
}

int read_from_file(FILE * fp, char * out){
	char buffer[256];
	char hex[2];
	char timer[256];
	int consumed;
	int accumulated;
	int i;
	if (fgets(buffer, sizeof(buffer), fp) != NULL){
		consumed = 0;
		accumulated = 0;
		i = 0;
		sscanf(buffer, "%[^|] %*[|]%n", timer, &consumed);	
		printf("%s\n", timer);
		accumulated = consumed;
		while (accumulated < strlen(buffer)){
			sscanf(buffer+accumulated, "%c%c%n", &hex[0], &hex[1], &consumed);
			out[i] = (char) hexToInt(hex);
			accumulated += consumed;
			i++;
		}
		return i;
	}else{
		fclose(fp);
		do_printf("File end. Closing file and exit");
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
	int size;
	int user_flag;
	char buffer[256];
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
		size = read_from_file(fp, buffer);
		if (size == NO_FRAME || size == NO_BEACON){
			/* Next iteration */
			continue;
		}
		err = kiss_ax25_unpack(buffer, size, &ax25);
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
