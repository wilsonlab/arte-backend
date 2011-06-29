import sys
import struct
import matplotlib.pyplot as plot
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

class RxArteUDP(DatagramProtocol):	
	
	def specifyBufferVars(self, trimTopN = 16, nSamples = 32, nChannels= 4, bufferSize = 128):
		self.trimTop = trimTopN
		self.nSamp = nSamples
		self.nChan = nChannels
		self.buffSize = bufferSize

	def datagramReceived(self, data, (host, port)):
		print "received from %s:%d " %(host, port)
		data = self.parseData(data)		
		self.plotData(data)

	def parseData(self,data):		
		wave = [None]*self.buffSize	
		for i in xrange(0,self.buffSize):
			idx = i*2 + self.trimTop
			wave[i] = (struct.unpack(">H", data[idx:idx+2]))[0]
		return wave

	def plotData(self, data):
		plot.plot(data)
		plot.show()
		return 
def main():
	port = 5000;
	print "Listening for packets on port:%d" %port
	rxArteUdp = RxArteUDP()	
	rxArteUdp.specifyBufferVars()
	reactor.listenUDP(port, rxArteUdp)
	print "Starting reactor press Ctrl-C to quit"
	reactor.run()	

if __name__ =="__main__":
	main()
