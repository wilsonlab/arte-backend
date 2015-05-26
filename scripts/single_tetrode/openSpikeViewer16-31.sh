#!/bin/bash

#specify starting port for spike viewer in next line. make sure it's +16 of the port specified in openSpikeViewer0-15
port=5228

#specify starting port for LFP viewer in next line
portlfp=7000

./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 16"
wmctrl -r "Tetrode 16" -e 0,0,0,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 17"
wmctrl -r "Tetrode 17" -e 0,688,0,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 18"
wmctrl -r "Tetrode 18" -e 0,1300,0,600,300

#2nd row

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 19"
wmctrl -r "Tetrode 19" -e 0,0,380,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 20"
wmctrl -r "Tetrode 20" -e 0,688,380,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 21"
wmctrl -r "Tetrode 21" -e 0,1300,380,600,300

#3rd row
port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 22"
wmctrl -r "Tetrode 22" -e 0,0,715,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 23"
wmctrl -r "Tetrode 23" -e 0,688,715,600,300

#lfp viewer
./arteLfpViewer <<<$portlfp >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network LFP Viewer" -N "LFP Viewer 15-23"
wmctrl -r "LFP Viewer 15-23" -e 0,1300,715,600,300

#screen 2 row 1
port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 24"
wmctrl -r "Tetrode 24" -e 0,2000,0,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 25"
wmctrl -r "Tetrode 25" -e 0,2614,0,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 26"
wmctrl -r "Tetrode 26" -e 0,3228,0,600,300

#screen 2 2nd row

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 27"
wmctrl -r "Tetrode 27" -e 0,2000,380,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 28"
wmctrl -r "Tetrode 28" -e 0,2614,380,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 29"
wmctrl -r "Tetrode 29" -e 0,3228,380,600,300

#screen 2 row 3
port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 30"
wmctrl -r "Tetrode 30" -e 0,2000,715,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 31"
wmctrl -r "Tetrode 31" -e 0,2614,715,600,300

#lfp viewer
portlfp=$((port+1))
./arteLfpViewer <<<$portlfp >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network LFP Viewer" -N "LFP Viewer 24-31"
wmctrl -r "LFP Viewer 24-31" -e 0,3228,715,600,300

