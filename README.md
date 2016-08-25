To compile:
rm -r build
mkdir build
cd build
cmake ..
make

The software will be compiled and a binary file can be found in "bin" folder.
To execute:
	printf("Usage: ./exec_name tcp_server_ip tcp_server_port\n");
	printf("Example: If listening a GNURadio TCP Server on localhost:\n");
	printf("\t./kiss 127.0.0.1 52001\n");
