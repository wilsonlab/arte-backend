CC = g++ --std=c++0x -g
LD = $(CC)

BUILD_DIR = ../../build
EXE = $(BUILD_DIR)/tracker

CFLAGS = -D__STDC_LIMIT_MACROS -pthread -std=c++0x -O0 -I /usr/local/include -I /usr/include/opencv2
CFLAGS += `pkg-config gtkmm-2.4 libglademm-2.4 --cflags`
CFLAGS += `pkg-config opencv --cflags` -I /usr/local/include/opencv2
CFLAGS += -I/usr/include/flycapture

LDFLAGS = -std=c++0x -ldl -lprotobuf -lboost_filesystem -lboost_system
LDFLAGS += `pkg-config opencv --libs`
LDFLAGS += `pkg-config gtkmm-2.4 libglademm-2.4 --libs`
LDFLAGS += -L../../lib -lflycapture

TRACKER_SRC = tracker.cpp ../arte_pb.pb.cc ../arte_command.pb.cc ../netcom.cpp ../datapacket.cpp
TRACKER_OBJ = tracker.o   ../arte_pb.pb.o  ../arte_command.pb.o  ../netcom.o   ../datapacket.o

TRACKER_SRC += tracker_pb.pb.cc tracker_data_source.cpp tracker_window.cpp
TRACKER_OBJ += tracker_pb.pb.o  tracker_data_source.o   tracker_window.o



.SUFFIXES:
.SUFFIXES: .o .pb.cc .cpp .cc .proto .h


$(EXE) : $(TRACKER_OBJ)
	$(LD) -o $@ $(TRACKER_OBJ) $(LDFLAGS)

../arte_pb.pb.cc : ../arte_pb.proto
	protoc -I ../ $< --cpp_out=../

../arte_command.pb.cc : ../arte_command.proto
	protoc -I ../ $< --cpp_out=../

tracker_pb.pb.cc tracker_pb.pb.h : tracker_pb.proto
	protoc $< --cpp_out=./

%.o : %.pb.cc
	$(CC) $(CFLAGS) -c $< -o $@


%.o : %.cpp
	$(CC) -c $< -o $@ $(CFLAGS) 


.PHONY : clean

clean :
	rm -f $(EXE)
	rm -f tracker.o
	rm -f tracker_pb.pb.cc tracker_pb.pb.h tracker_pb.pb.o
	rm -f tracker_data_source.o tracker_processor.o