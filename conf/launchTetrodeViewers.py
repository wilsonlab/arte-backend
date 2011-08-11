import string, os, argparse, subprocess
from xml.etree.ElementTree import ElementTree

executable = "../scripts/arteSpikeViewer"
 
xmin = 0;
ymin = 0;
xmax = 1600;
ymax = 1200;
nrow = 4;
ncol = 4;
ypad = 30

xpos = xmin
ypos = ymin

dx = xmax / ncol
dy = ymax / nrow

tetrodeChan = {}
tetrodePort = {}

pid = {}

def parseArgs():
	parser = argparse.ArgumentParser()
	parser.add_argument('-x', '--xmin')
	parser.add_argument('-y', '--ymin')
	parser.add_argument('-X', '--xmax')
	parser.add_argument('-Y', '--ymax')
	parser.add_argument('-c', '--ncol')
	parser.add_argument('-r', '--nrow')
	args = parser.parse_args()
	xmin = int(args.xmin)
	ymin = int(args.ymin)

	xmax = int(args.xmax)
	ymax = int(args.ymax)

	nrow = int(args.nrow)
	ncol = int(args.ncol)

	xpos = xmin
	ypos = ymin

	dx = xmax / ncol
	dy = ymax / nrow


def parseXml():
	xmlfile = "arte_session_default.conf";

	tree = ElementTree();
	tree.parse(xmlfile);

	tets = tree._root.find("session").find("trodes").getchildren();

	for tet in tets:
		name = string.strip(tet.text)
		tetrodeChan[name] = tet.find('channels').text;
		tetrodePort[name] = tet.find('port').text;


def launchViewers():
	countx = 1
	county = 1
	countp = 1

	for n in tetrodeChan.iterkeys():
		cmd = executable
		cmd = cmd + " --port" + tetrodePort[n]
		cmd = cmd + " --windowname \"" + "Tetrode:" + str(n) + " Channels:" + tetrodeChan[n] + "\""
		cmd = cmd + " --xposition " + str(xpos)
		cmd = cmd + " --yposition " + str(ypos)
		cmd = cmd + " --width " + str(dx)
		cmd = cmd + " --height " + str(dy)
#	cmd = cmd + " &"

	if countx < ncol:
		xpos += dx
		countx += 1

	else:
		xpos = 0
		ypos += dy + (ypad * county)

		countx = 0
		county +=1

	print cmd;
#	os.system(cmd)
	pid[countp] = subprocess.Popen(cmd, shell=True)
	countp = countp+1


input = '1'

while input!='X':
	input = raw_input("Press Enter to continue...")
	print input
