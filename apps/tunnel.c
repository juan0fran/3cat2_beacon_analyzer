#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <socket_utils.h>
#include <packet.h>
#include <signal.h>
#include <poll.h>
#include <time.h>

#define MAX_RANDOM  ((1L << 31L) - 1L)
/*#define PER 		15L*/
int HasToBeSent(long PER){
	long n;
	do {
	    n = random();
	} while (n > (MAX_RANDOM - ((MAX_RANDOM % 100) + 1) % 100));
	n %= 100;
	printf("Random number: %ld from PER: %ld\n\n", n, PER);
	if (n > PER){
		/* nop */
		return 1;
	}else{
		/* yup */
		return 0;
	}
}

int main(int argc, char ** argv){
	int plen;
	int len;
	int rv;
	long p;
	char *str, *endptr;
	struct pollfd ufds[2];
	BYTE buffer[MTU_SIZE + MTU_OVERHEAD];

	if (argc != 4){
		exit(-1);
	}
	int phy1 = initialise_client_socket(argv[1]);
	int phy2 = initialise_client_socket(argv[2]);
	str = argv[3];
	p = strtol(str, &endptr, 10);
	if (str == endptr){
		printf("No PER found please put as 4th argument\n");
	}
	printf("Client has been initialised\n");
 	srandom((unsigned)time(NULL));

	while(1){
		ufds[0].fd = phy1;
		ufds[0].events = POLLIN; // check for normal data
		ufds[1].fd = phy2;
		ufds[1].events = POLLIN; // check for normal data
		rv = poll(ufds, 2, -1);
		if (rv == -1){
			perror("poll");
			/* just return ... */
		}else if(rv == 0){
			printf("What!?\n"); /* death link timeout, try to check link availability */
		}else{
			printf("Received some event\n");
			if (ufds[1].revents & POLLIN){
				if (read(ufds[1].fd, &plen, sizeof(int)) == 0 ){
					exit(1);
				}
				printf("plen from 1 is: %d\n", plen);
				len = 0;
				if (plen > MTU_SIZE + MTU_OVERHEAD){
					continue;
				}
				while(len != plen){
					len += read(ufds[1].fd, buffer, plen);
				}
				printf("Received from 1: %s\n", buffer+3);
				/* Sometimes i do not want to write */
				if (HasToBeSent(p)){
					write(ufds[0].fd, &plen, sizeof(int));
					write(ufds[0].fd, buffer, plen);
				}
			}
			if (ufds[0].revents & POLLIN){
				if (read(ufds[0].fd, &plen, sizeof(int)) == 0){
					exit(1);
				}
				printf("plen from 0 is: %d\n", plen);
				len = 0;
				if (plen > MTU_SIZE + MTU_OVERHEAD){
					continue;
				}
				while(len != plen){
					len += read(ufds[0].fd, buffer, plen);
				}
				printf("Received from 0: %s\n", buffer+3);
				/* Sometimes i do not want to write */
				if (HasToBeSent(p)){
					write(ufds[1].fd, &plen, sizeof(int));
					write(ufds[1].fd, buffer, plen);
				}
			}
			/* We write the packet, but we simulate a delay */
			/* Transmission time is aprox 250 ms */
			usleep(10 * 1000);
		}
	}
}


