#!/bin/bash

#specify starting port for spike viewer in next line
port=5228

#specify starting port for LFP viewer in next line
portlfp=7000

./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 0"
wmctrl -r "Tetrode 0" -e 0,0,0,600,300

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 1"
wmctrl -r "Tetrode 1" -e 0,688,0,600,300

port=$((port+2))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 2"
wmctrl -r "Tetrode 2" -e 0,1300,0,600,300

#2nd row

port=$((port+3))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 3"
wmctrl -r "Tetrode 3" -e 0,0,380,600,300

port=$((port+4))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 4"
wmctrl -r "Tetrode 4" -e 0,688,380,600,300

port=$((port+5))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 5"
wmctrl -r "Tetrode 5" -e 0,1300,380,600,300

#3rd row
port=$((port+6))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 6"
wmctrl -r "Tetrode 6" -e 0,0,715,600,300

port=$((port+7))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 7"
wmctrl -r "Tetrode 7" -e 0,688,715,600,300

#lfp viewer
./arteLfpViewer <<<$portlfp >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network LFP Viewer" -e 0,1300,715,600,300
 
