#!/usr/bin/env python
import pygtk, gtk, gtk.glade
pygtk.require("2.0")

import os, string, subprocess, signal
from xml.etree.ElementTree import ElementTree

class ViewerLauncher(object):
	def __init__(self):

		self.bin = ""
		self.dataChan = {}
		self.dataPort = {}
		self.dataLabel = {}	
		self.pid = {}

		self.xMin = -1
		self.xMax = -1
		self.yMin = -1
		self.yMax = -1

		self.xPad = -1
		self.yPad = -1

		self.nRows = -1
		self.nCols = -1

	def setBinary(self, bin):
		self.bin = bin

	def setGuiParams(self, xMin, xMax, yMin, yMax, xPad, yPad, nRows, nCols):
		self.xMin = xMin
		self.xMax = xMax
		self.yMin = yMin
		self.yMax = yMax
		self.xPad = xPad
		self.yPad = yPad
		self.nRows = nRows
		self.nCols = nCols

	def parseArteConfigFile(self, configFile):	
		print "not implemented in the parent class"

	def launchViewers(self):
		countx = 1
		county = 1
		countp = 1
		
		xpos = self.xMin
		ypos = self.yMin
	
		dx = (self.xMax - self.xMin) / self.nCols
		dy = (self.yMax - self.yMin) / self.nRows
		
		for n in self.dataChan.iterkeys():
			cmd = self.bin
			cmd = cmd + " --port" + self.dataPort[n]
			cmd = cmd + " --windowname \"" + "Tetrode:" + self.dataLabel[n] + "\""
			cmd = cmd + " --xposition " + str(xpos)
			cmd = cmd + " --yposition " + str(ypos)
			cmd = cmd + " --width " + str(dx - self.xPad)
			cmd = cmd + " --height " + str(dy - self.yPad)

			if countx < self.nCols:
				xpos += dx
				countx += 1

			else:
				xpos = 0
				ypos += dy #+ (self.yPad * county)

				countx = 0
				county +=1

			print cmd
			self.pid[countp] = subprocess.Popen(cmd, shell=True)
			countp = countp+1

		return self.pid

class ArteSpikeViewerLauncher(ViewerLauncher):
	def parseArteConfigFile(self, configFile):	
		print "parsing arte config file"
		tree = ElementTree()
		tree.parse(configFile)
		tets = tree._root.find("session").find("trodes").getchildren()

		for tet in tets:
			n = string.strip(tet.text)
			self.dataChan[n] = tet.find('channels').text
			self.dataPort[n] = tet.find('port').text
			#self.ttLabel[n]= tet.find('label').text
			self.dataLabel[n] = str(n) + " Channels:" + self.dataChan[n]


class ArteWaveViewerLauncher(ViewerLauncher):
	def parseArteConfigFile(self, configFile):	
		print "parsing arte config file"
		tree = ElementTree()
		tree.parse(configFile)
		lfps = tree._root.find("session").find("lfp_banks").getchildren()

		for lfp in lfps:
			n = string.strip(lfp.text)
			self.dataChan[n] = lfp.find('channels').text
			self.dataPort[n] = lfp.find('port').text
			#self.ttLabel[n]= tet.find('label').text
			self.dataLabel[n] = str(n) + " Channels:" + self.dataChan[n]

