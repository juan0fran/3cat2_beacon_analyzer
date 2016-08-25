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
#include <stdarg.h>

typedef enum PrintType{
	RAW,
	FORMATTED,
}PrintType;

static PrintType flag = false;

void initiate_printf(int print_flag){
	if (print_flag == 0){
		flag = RAW;
	}else{
		flag = FORMATTED;
	}
}

void do_printf(char *fmt, ...){
	va_list args;
	char tmp_msg[256];
	if (flag == RAW){
		return;
	}else{
    	va_start(args, fmt);
    	vsnprintf(tmp_msg, 255, fmt, args);
    	va_end(args);
    	printf("%s", tmp_msg);
	}
}