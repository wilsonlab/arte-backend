clear
rm test7
rm test.txt
rm *.filt
rm *.unfilt
g++ src/arte.cpp src/arteopt.cpp src/filt.cpp src/neural_daq.cpp src/trode.cpp src/util.cpp src/lfp_bank.cpp src/filtered_buffer.cpp src/filter_fun.cpp src/timer.cpp src/process_command.cpp src/netcom.cpp src/datapacket.cpp -pthread -ldl -lnidaqmx -I /usr/local/boost_1_45_0/ -I src/ -D__STDC_LIMIT_MACROS -O3 -o build/arte_backend