#Beacon analyzer for CUBECAT-2 (3CAT-2) mission. It can receive a stream from GNURadio TCP Server or from a file using netcat (nc).

*The GNURadio receiver and demodulator can be found at http://destevez.net/2016/08/decoding-packets-from-3cat2*


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
