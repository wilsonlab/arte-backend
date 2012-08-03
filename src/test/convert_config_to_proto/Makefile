CC = g++
LD = $(CC)

CFLAGS = -D__STDC_LIMIT_MACROS -std=c++0x -I $(wildcard /usr/local/boost_1_45_0) -I $(wildcard /usr/local/boost) -I ../.. -pthread
LDFLAGS = -std=c++0x -lprotobuf -pthread -lprotobuf -lz -lpthread
SRCS = ../..

.SUFFIXES:
.SUFFIXES: .o .c .cpp .cc .proto .h

convert_config_to_proto : convert_config_to_proto.o arte_pb.pb.o arte_command.pb.o glom.o
	$(LD) -o $@ $^ $(LDFLAGS)

convert_config_to_proto.o : convert_config_to_proto.cpp
	$(CC) -c $< $(CFLAGS)

glom.o : ../../glom.cpp
	$(CC) -c $< $(CFLAGS)

%.o : %.cc
	$(CC) -c $< $(CFLAGS) -o $@

arte_pb.pb.cc : ../../arte_pb.proto
	protoc $< -I ../.. --cpp_out=./

arte_command.pb.cc : ../../arte_command.proto
	protoc $< -I ../.. --cpp_out=./