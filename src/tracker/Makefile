CC = g++
LD = $(CC)

BUILD_DIR = ../build
EXE = $(BUILD_DIR)/tracker

CFLAGS = -D__STDC_LIMIT_MACROS -pthread -std=c++0x -O0 -I /usr/local/include
LDFLAGS = -std=c++0x -ldl -lprotobuf

TRACKER_SRC = tracker.cpp ../arte_pb.pb.cc ../arte_command.pb.cc ../netcom.cpp ../datapacket.cpp
TRACKER_OBJ = tracker.o   ../arte_pb.pb.o  ../arte_command.pb.o  ../netcom.o   ../datapacket.o

TRACKER_SRC += tracker_pb.pb.cc
TRACKER_OBJ += tracker_pb.pb.o

.SUFFIXES:
.SUFFIXES: .o .pb.cc .cpp .cc .proto .h


$(EXE) : $(TRACKER_OBJ)
	LD -o $@ $(TRACKER_OBJ) $(LDFLAGS)

../arte_pb.pb.o : ../arte_pb.proto
	protoc -I ../ $< --cpp_out=../

../arte_command.pb.o : ../arte_command.proto
	protoc -I ../ $< --cpp_out=../

tracker_pb.pb.o : tracker_pb.proto
	protoc $< --cpp_out=./

%.o : %.pb.cc
	$(CC) $(CFLAGS) -c $< -o $@


%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


