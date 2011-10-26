clear
rm test7
rm test.txt
rm *.filt
rm *.unfilt
g++ src/backend/arte.cpp src/backend/arteopt.cpp src/backend/filt.cpp src/backend/neural_daq.cpp src/backend/trode.cpp src/common/util.cpp src/backend/lfp_bank.cpp src/backend/filtered_buffer.cpp src/backend/filter_fun.cpp src/timer/timer.cpp src/backend/process_command.cpp src/netcom/netcom.cpp src/netcom/datapacket.cpp -pthread -ldl -lnidaqmx -I /usr/local/boost_1_45_0/ -I include/backend -I include/common -I include/timer -I include/netcom -D__STDC_LIMIT_MACROS -O3 -o build/arte_backend