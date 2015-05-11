import socket
import sys

if len(sys.argv) != 4:
	sys.exit("Usage: forwardUdp PORT_IN IP_OUT PORT_OUT")
ip_in  = "127.0.0.1"


port_in = int(sys.argv[1]);
ip_out = sys.argv[2]
port_out = int(sys.argv[3]);

print "Sending packets received on:", port_in, " to IP:", ip_out, " port:", port_out

sock_in = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
sock_in.bind( (ip_in, port_in) )

sock_out = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )

while True:
	data, addr = sock_in.recvfrom(1024)
	sock_out.sendto( data, (ip_out, port_out) )


