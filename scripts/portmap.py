#!/usr/bin/python

# portmap.py - threaded script listens to a number of
# ports, forwards all incoming packets to given output
# ports.  Inputs are the first column in portmap.txt,
# and outputs are all subsequent columns
#
# (forwarding within rows only)
# 5227 -> 6227 and 7227
# 5228 -> 6228 and 7228 etc.

import threading
import thread
import sys
import socket
import time

# Packet forwarder isa thread object for forwarding 
# from a single port to multiple ports
class packet_forwarder(threading.Thread):

    def __init__(self, ports_string):
        print "Init called"
        print "ports_string: " + ports_string
        ports_list = ports_string.split(' ')
        self.in_port = ports_list[0]
        self.out_ports = ports_list[1:]
        self.have_data = 0
        threading.Thread.__init__(self)
        self.init_ports()

    def init_ports(self):
        self.sock_in = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
        print self.in_port
        self.sock_in.settimeout(2)
        print "in_ip: " + in_ip + "int(self.in_port)::" 
        print int(self.in_port)
        self.sock_in.bind ( ("", int(self.in_port)) )
        self.sock_out = range(0, len(self.out_ports) )
        for i in range(0, len(self.out_ports)):
                self.sock_out[i] = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )

    def hello1(self):
        print "Hello from thread!"
        print "In Ports"
        print "_" + self.in_port + "_"
        print "Out Ports"
        for i in self.out_ports:
            print "_" + i + "_"

    def run(self):
        print "Run called."
        self.hello1()
        while running:
            try:
                #print "Trying."
                data, addr = self.sock_in.recvfrom(1024)
                #print "GOT ONE! Port: " + self.in_port
                self.have_data = 1
            except:
                self.have_data = 0
                print "Caught and ignored a timeout exception"
            if self.have_data:
                for i in range(0, len(self.sock_out)):
                    for m in range(0, len(out_ip)):
                        self.sock_out[i].sendto( data, (out_ip[m], int(self.out_ports[i])) )
        print "Finished while running loop."

    
n_out_ip = len(sys.argv) -1  # all but first arg, interpret as out_ip
out_ip = []

if (n_out_ip == 0):
    out_ip.append( "127.0.0.1" )
else:
    for m in range(1, len(sys.argv)):
        out_ip.append( sys.argv[m])

running = 1
in_ip = ""

f = open('./portmap.txt','r')

packet_forwarders = []
    
for line in f:
    line = line[0:(len(line)-1)]
    if( line[0] != '#' ):
        print "Opening with line: _" + line + "_"
        
        t1 = packet_forwarder(line)
        packet_forwarders.append( t1 );
        t1.start()
        
raw_input("Press Enter to quit.")
running = 0
