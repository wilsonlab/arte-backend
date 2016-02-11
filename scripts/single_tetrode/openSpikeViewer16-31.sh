#!/bin/bash

#specify starting port for spike viewer in next line. make sure it's +16 of the port specified in openSpikeViewer0-15
port=5316

#specify starting port for LFP viewer in next line
portlfp=5202

./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 16"
wmctrl -r "Tetrode 16" -e 0,10,10,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 17"
wmctrl -r "Tetrode 17" -e 0,645,10,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 18"
wmctrl -r "Tetrode 18" -e 0,1280,10,620,310

#2nd row

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 19"
wmctrl -r "Tetrode 19" -e 0,10,355,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 20"
wmctrl -r "Tetrode 20" -e 0,645,355,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 21"
wmctrl -r "Tetrode 21" -e 0,1280,355,620,310

#3rd row
port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 22"
wmctrl -r "Tetrode 22" -e 0,10,700,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 23"
wmctrl -r "Tetrode 23" -e 0,645,700,620,310

#lfp viewer
./arteLfpViewer <<<$portlfp >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network LFP Viewer" -N "LFP Viewer 15-23"
wmctrl -r "LFP Viewer 15-23" -e 0,1280,700,620,310

#screen 2 row 1
port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 24"
wmctrl -r "Tetrode 24" -e 0,1940,10,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 25"
wmctrl -r "Tetrode 25" -e 0,2570,10,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 26"
wmctrl -r "Tetrode 26" -e 0,3200,10,620,310

#screen 2 2nd row

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 27"
wmctrl -r "Tetrode 27" -e 0,1940,355,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 28"
wmctrl -r "Tetrode 28" -e 0,2570,355,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 29"
wmctrl -r "Tetrode 29" -e 0,3200,355,620,310

#screen 2 row 3
port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 30"
wmctrl -r "Tetrode 30" -e 0,1940,700,620,310

port=$((port+1))
./arteSpikeViewer <<<$port >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network Spike Viewer" -N "Tetrode 31"
wmctrl -r "Tetrode 31" -e 0,2570,700,620,310

#lfp viewer
portlfp=$((portlfp+1))
./arteLfpViewer <<<$portlfp >/dev/null 2>/dev/null &
sleep 1
wmctrl -r "Arte Network LFP Viewer" -N "LFP Viewer 24-31"
wmctrl -r "LFP Viewer 24-31" -e 0,3200,715,620,310

