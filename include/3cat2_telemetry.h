#ifndef __3CAT2_TELEMETRY_H__
#define __3CAT2_TELEMETRY_H__

#define FEND	0xC0
#define FESC	0xDB
#define TFEND	0xDC
#define TFESC	0xDD

typedef enum ErrorHandler{
	NO_ERROR = -1,
	NO_FRAME = -2,
	NO_BEACON = -3,
}ErrorHandler;

typedef struct ax25_packet_t{
	char from[7];
	char to[7];
	char control;
	char pid;
	char * info; /* Needs to be malloc'ed */
	char size;
}ax25_packet_t;

int read_kiss_from_socket(int fd, char * buffer);
ErrorHandler kiss_ax25_unpack(char * buffer, int size, ax25_packet_t * out);
ErrorHandler decode_3cat2_packet(ax25_packet_t * in);
ErrorHandler rawprint_3cat2_packet(ax25_packet_t * in);

#endif