#!/usr/bin/env python
import pygtk, gtk, gtk.glade, pango
pygtk.require("2.0")

import os, string, subprocess, signal
from ViewerLaunchers import ArteSpikeViewerLauncher as SpikeLauncher
from ViewerLaunchers import ArteWaveViewerLauncher as WaveLauncher

from xml.etree.ElementTree import ElementTree


class ArteViewerLauncher:
	def __init__(self):

		self.curDir = os.path.dirname(os.path.realpath(__file__)) + "/"

		self.spikebin = self.curDir + '../scripts/arteSpikeViewer'
		self.wavebin = self.curDir + '../scripts/arteLfpViewer'
		self.arteConfigFile = self.curDir + '../conf/arte_session_default.conf'
		self._createLaunchers()

		self.gladeFile = self.curDir+"masterlauncher.glade"
		self.wTree = gtk.glade.XML(self.gladeFile)
		self._initGtkWidgets()
		self._initGuiParams()

		self.mainWindow.show()

	def _initGtkWidgets(self):
		self.mainWindow = self.wTree.get_widget("MainWindow")
		self.mainWindow.resize(275,106)
		self.mainWindow.set_icon_from_file( self.curDir + '../share/Arte.png')
		self.controlWindow = self.wTree.get_widget("ControllerWindow")
		self.controlWindow.set_icon_from_file( self.curDir + '../share/Arte.png')
		
		self.mainWindow.connect("delete_event", self._shutDownFn)
		self.controlWindow.connect("delete_event", self._shutDownFn)

		self.launchBtn = self.wTree.get_widget("launchBtn")
		self.launchBtn.connect("clicked", self.launchBtnFn)
	
		self.optionsBox = self.wTree.get_widget("optionsHbox")
		self.toggleOptBtn = self.wTree.get_widget("toggleAdvancedBtn")
		self.toggleOptBtn.connect("clicked", self.toggleOptBtnFn)

		self.clearBtn = self.wTree.get_widget("clearBtn")
		self.pauseBtn = self.wTree.get_widget("pauseBtn")
		self.closeBtn = self.wTree.get_widget("closeBtn")

		self.clearBtn.connect("clicked", self.clearBtnFn)
		self.pauseBtn.connect("clicked", self.pauseBtnFn)
		self.closeBtn.connect("clicked", self.closeBtnFn)

		largeFont = pango.FontDescription("sans 18")
		self.wTree.get_widget("tetrodeMainLbl").modify_font(largeFont)
		self.wTree.get_widget("lfpMainLbl").modify_font(largeFont)
		self.wTree.get_widget("cmdRouterLbl").modify_font(largeFont)
		self.launchBtn.get_child().modify_font(largeFont)

	def _initGuiParams(self):
		self.tetPid = {}

		self.tetXMin = -1
		self.tetXMax = -1
		self.tetYMin = -1
		self.tetYMax = -1

		self.tetXPad = -1
		self.tetYPad = -1

		self.tetRows = -1
		self.tetCols = -1

		self.lfpPid = {}

		self.lfpXMin = -1
		self.lfpYMax = -1
		self.lfpYMin = -1
		self.lfpYMax = -1

		self.lfpXPad = -1
		self.lfpYPad = -1

		self.lfpRows = -1
		self.lfpCols = -1

	def _createLaunchers(self):
		self.tetLauncher = SpikeLauncher()
		self.lfpLauncher = WaveLauncher()

		self.tetLauncher.setBinary(self.spikebin)
		self.lfpLauncher.setBinary(self.wavebin)

	def launchBtnFn(self, widget):

		print "Grabbing user input"
		self.getEntryValues()

		validInputs = self.checkInputs()
		print "User inputs are valid:", validInputs
		if not validInputs:
			return
	
		self._parseArteConfig()

		self.tetLauncher.setGuiParams( \
			self.tetXMin, self.tetXMax, self.tetYMin, self.tetYMax, \
			self.tetXPad, self.tetYPad, self.tetRows, self.tetCols)

		self.lfpLauncher.setGuiParams( \
			self.lfpXMin, self.lfpXMax, self.lfpYMin, self.lfpYMax, \
			self.lfpXPad, self.lfpYPad, self.lfpRows, self.lfpCols)

		print "Launching Viewers"

		print "Showing Master Controler"
		self.mainWindow.hide()
		self.controlWindow.show()
		self._launchViewers()

	def getEntryValues(self):
		self.tetXMin  = self.getInput("xMinSpikeTxt")
		self.tetYMin  = self.getInput("yMinSpikeTxt")
		self.tetXMax  = self.getInput("xMaxSpikeTxt")
		self.tetYMax  = self.getInput("yMaxSpikeTxt")
		self.tetXPad  = self.getInput("xPadSpikeTxt")
		self.tetYPad  = self.getInput("yPadSpikeTxt")
		self.tetRows = self.getInput("nRowsSpikeTxt")
		self.tetCols = self.getInput("nColsSpikeTxt")

		self.lfpXMin  = self.getInput("xMinLfpTxt")
		self.lfpYMin  = self.getInput("yMinLfpTxt")
		self.lfpXMax  = self.getInput("xMaxLfpTxt")
		self.lfpYMax  = self.getInput("yMaxLfpTxt")
		self.lfpXPad  = self.getInput("xPadLfpTxt")
		self.lfpYPad  = self.getInput("yPadLfpTxt")
		self.lfpRows = self.getInput("nRowsLfpTxt")
		self.lfpCols = self.getInput("nColsLfpTxt")

#		self.arteConfigFile = self.getInput("cfgFileChooser")

	def setEntryValues(self):
		print "Not Yet Implemented"		


	def getInput(self, widgetName):
		if widgetName == 'cfgFileChooser':
			widget = self.wTree.get_widget(widgetName)
			f = widget.get_file()
			if not f == None:
				return f.get_path()
			else:
				print "No Arte configuration file specified, you must pick one"
				return "noFile"

		retVal = -1
		try:
			retVal = int(self.wTree.get_widget(widgetName).get_text())
		except ValueError:
			print "Parsing Error for Widget:", widgetName, 'its value must be a non negative integer'
		if retVal<0:
			retVal = -1
		return retVal


	def checkInputs(self):
		return 	self.tetXMin >= 0 and self.tetYMin >=0 and\
				self.tetXMax >0 and self.tetYMax >0 and\
				self.tetXPad >= 0 and self.tetYPad >=0 and\
				self.tetRows >= 0 and self.tetCols >=0 and\
			 	self.lfpXMin >= 0 and self.lfpYMin >=0 and\
				self.lfpXMax >0 and self.lfpYMax >0 and\
				self.lfpXPad >= 0 and self.lfpYPad >=0 and\
				self.lfpRows >= 0 and self.lfpCols >=0 and\
				os.path.exists(self.arteConfigFile)


	def _parseArteConfig(self):	
		self.tetLauncher.parseArteConfigFile(self.arteConfigFile)
		self.lfpLauncher.parseArteConfigFile(self.arteConfigFile)

	def _launchViewers(self):
		print "Launcing Viewers"
		self.tetPid = self.tetLauncher.launchViewers()
		self.lfpPid = self.lfpLauncher.launchViewers()


	def clearBtnFn(self, widget):
		self._clearViewers()


	def pauseBtnFn(self, widget):
		self._pauseViewers()


	def closeBtnFn(self, widget):
		self._killViewers()
		self.controlWindow.hide();
		self.mainWindow.show();

	def toggleOptBtnFn(self, widget):

		isVis = not self.optionsBox.get_visible()
		self.optionsBox.set_visible(isVis)
		print self.mainWindow.get_size()

		if isVis:
			widget.get_child().set_text("Fewer Options")
		else:
			widget.get_child().set_text("More Options")
			self.mainWindow.resize(275,106) # Shrink the window

	def _killViewers(self):
		print "Killing any open viewers"
		for i in xrange(1, len(self.tetPid)+1):
			print "Killing ", i
			self.tetPid[i].send_signal(signal.SIGTERM)

		for i in xrange(1, len(self.lfpPid)+1):
			self.lfpPid[i].send_signal(signal.SIGTERM)


	def _clearViewers(self):
		print "Clearing Viewers"
		for i in xrange(1, len(self.tetPid)+1):
			self.tetPid[i].send_signal(signal.SIGUSR1)


	def _pauseViewers(self):
		print "Pausing Viewers"

	def _shutDownFn(self, *args):
		self._killViewers()
		gtk.main_quit()

if __name__=="__main__":
	avl = ArteViewerLauncher()
	gtk.main()
