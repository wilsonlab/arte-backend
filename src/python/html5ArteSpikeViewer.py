import matplotlib
matplotlib.use('module://mplh5canvas.backend_h5canvas')
from pylab import *
import time

import sys
import struct
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

MAX_VOLTAGE = 1500;

class RxArteUDP(DatagramProtocol):	

	def __init__(self):
		
		self.packetCount = 0;
		self.specifyBufferVars()	
#		self.ax = p.subplot(111)
#		self.canvas = self.ax.figure.canvas
#		self.line, = p.plot(xrange(0,128), xrange(0,128), animated=True);		
#		self.bg = self.canvas.copy_from_bbox(self.ax.bbox)
#		p.draw()

		self.t = xrange(0,self.buffSize)
		s = xrange(0,self.buffSize)
		plot(self.t, s, linewidth=1.0)
		ylabel('voltage (ad units)')
		title('ARTE HTML5 Spike Viewer')
		self.f = gcf()
		self.ax = self.f.gca()
		self.ax.set_xlim(0, 128)
		self.ax.set_ylim(0, MAX_VOLTAGE)
		show(block=False)

	def specifyBufferVars(self, trimTopN = 16, nSamples = 32, nChannels= 4, bufferSize = 128):
		self.trimTop = trimTopN
		self.nSamp = nSamples
		self.nChan = nChannels
		self.buffSize = bufferSize

	def datagramReceived(self, data, (host, port)):
		print "received from %s:%d " %(host, port)
		self.packetCount = self.packetCount + 1
		data = self.parseData(data)		
		self.plotData(data)
	

	def parseData(self,data):		
		wave = [None]*self.buffSize	
		for i in xrange(0,self.buffSize):
			idx = i*2 + self.trimTop
			wave[i] = (struct.unpack(">H", data[idx:idx+2]))[0]
		wave[0] = MAX_VOLTAGE * (self.packetCount%30) / 30
		wave[1] = wave[0]
		return wave

	def plotData(self, data):
		self.ax.lines[0].set_xdata(self.t)
		self.ax.lines[0].set_ydata(data)
		self.f.canvas.draw()
		#self.line.set_ydata(data)
		return 
def main():
	port = 5000;
	print "Listening for packets on port:%d" %port
	rxArteUdp = RxArteUDP()	
	reactor.listenUDP(port, rxArteUdp)
	print "Starting reactor press Ctrl-C to quit"
	reactor.run()	

if __name__ =="__main__":
	main()
