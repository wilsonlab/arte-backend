BUILD_DIR = build/
SOURCE_DIR = src/

BACKEND_SRC = src/arte.cpp src/filt.cpp src/neural_daq.cpp src/trode.cpp src/util.cpp src/lfp_bank.cpp src/filtered_buffer.cpp src/filter_fun.cpp src/timer.cpp src/process_command.cpp src/netcom.cpp src/datapacket.cpp src/arte_pb.pb.cc

build/arte_backend: $(BACKEND_SRC)
	g++ $(BACKEND_SRC) -pthread -ldl -lnidaqmx -I /usr/local/boost_1_45_0/ -I src/ -D__STDC_LIMIT_MACROS -O3 -o build/arte_backend

src/arte_pb.pb.cc: src/arte_pb.proto
	protoc src/arte_pb.proto --cpp_out=./src/