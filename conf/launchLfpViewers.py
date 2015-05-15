#/usr/bin/env python
import string, os, argparse
from xml.etree.ElementTree import ElementTree

parser = argparse.ArgumentParser()
parser.add_argument('-x', '--xmin')
parser.add_argument('-y', '--ymin')
parser.add_argument('-X', '--xmax')
parser.add_argument('-Y', '--ymax')
parser.add_argument('-c', '--ncol')
parser.add_argument('-r', '--nrow')
args = parser.parse_args()

xmlfile = "arte_session_default.conf";

print "Parsing:" + xmlfile
tree = ElementTree();
tree.parse(xmlfile);

lfps = tree._root.find("session").find("lfp_banks").getchildren();

lfpChan = {}
lfpPort = {}
for l in lfps:
	name = string.strip(l.text)
	lfpChan[name] = l.find('channels').text;
	lfpPort[name] = l.find('port').text;

xmin = int(args.xmin)
ymin = int(args.ymin)

xmax = int(args.xmax)
ymax = int(args.ymax)

nrow = int(args.nrow)
ncol = int(args.ncol)

ypad = 30

xpos = xmin
ypos = ymin

dx = (xmax - xmin) / ncol
dy = (ymax - ymin) / nrow

binary = "../scripts/single_tetrode/arteLfpViewer"
countx = 1
county = 1
for n in lfpChan.iterkeys():
	print n
	cmd = binary
	cmd = cmd + " --port" + lfpPort[n]
	cmd = cmd + " --windowname \"" + "lfp:" + str(n) + " Channels:" + lfpChan[n] + "\""
	cmd = cmd + " --xposition " + str(xpos)
	cmd = cmd + " --yposition " + str(ypos)
	cmd = cmd + " --width " + str(dx)
	cmd = cmd + " --height " + str(dy)
	cmd = cmd + " &"

	if countx < ncol:
		xpos += dx
		countx += 1

	else:
		xpos = 0
		ypos += dy + (ypad * county)

		countx = 0
		county +=1

	print cmd;
	os.system(cmd)

