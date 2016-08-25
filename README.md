To compile:<br/>
rm -r build<br/>
mkdir build<br/>
cd build<br/>
cmake ..<br/>
make<br/>

The software will be compiled and a binary file can be found in "bin" folder.<br/>
To execute:<br/>
	printf("Usage: ./exec_name tcp_server_ip tcp_server_port\n");<br/>
	printf("Example: If listening a GNURadio TCP Server on localhost:\n");<br/>
	printf("\t./kiss 127.0.0.1 52001\n");<br/>
