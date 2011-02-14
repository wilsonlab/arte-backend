clear
rm test6
rm test.txt
g++ src/arte.cpp src/arteopt.cpp src/filt.cpp src/neural_daq.cpp src/trode.cpp src/util.cpp src/spike_detector.cpp -pthread -ldl -lnidaqmx -I /usr/local/boost_1_45_0/ -g -o test6