# this file gets included by the one in the main project directory,
# so act as though we are there, not in src/
CC = g++44
LD = $(CC)
# in next line: don't nesceccarily do -lnidaqmx.  Instead, check for it and conditional link, as in
# this post (http://stackoverflow.com/questions/3910054/makefile-conditional-include)
LDFLAGS = -ldl -lnidaqmx -lprotobuf -lzmq
BUILD_DIR = build/
NIDAQMX_PATH = /usr/local/include/NIDAQmx.h
SRCS = src/
BACKEND_SRC = $(SRCS)arte.cpp $(SRCS)filt.cpp $(SRCS)neural_daq.cpp $(SRCS)trode.cpp $(SRCS)util.cpp $(SRCS)lfp_bank.cpp $(SRCS)filtered_buffer.cpp $(SRCS)filter_fun.cpp $(SRCS)timer.cpp $(SRCS)process_command.cpp $(SRCS)netcom.cpp $(SRCS)datapacket.cpp $(SRCS)arte_pb.pb.cc $(SRCS)arte_pb.pb.h $(SRCS)arte_command.pb.cc $(SRCS)arte_command.pb.h $(SRCS)arteopt.cpp $(SRCS)arte_command_port.cpp $(SRCS)a_data_source.cpp $(SRCS)nidaq_data_source.cpp

BACKEND_OBJ = $(SRCS)process_command.o $(SRCS)arteopt.o  $(SRCS)arte_pb.pb.o $(SRCS)arte_command.pb.o $(SRCS)filt.o $(SRCS)neural_daq.o $(SRCS)trode.o $(SRCS)util.o $(SRCS)lfp_bank.o $(SRCS)filtered_buffer.o $(SRCS)filter_fun.o $(SRCS)timer.o  $(SRCS)netcom.o $(SRCS)datapacket.o $(SRCS)arte.o $(SRCS)arte_command_port.o

# path to boost library added to 
CFLAGS = -pthread -std=c++0x -I $(SRCS) -D__STDC_LIMIT_MACROS -O3 -I /usr/local/include/ -I $(wildcard /usr/local/boost_1_45_0) -I $(wildcard /usr/local/boost-trunk) -I $(wildcard /usr/local/boost_1_45_0/boost/property_tree)


OBJS = src/
RM = rm
EXE = $(BUILD_DIR)arte_backend

# clear out all suffixes (from http://owen.sj.ca.us/~rk/howto/slides/make/slides/makesuff.html)
# should this and the .c.o: rule be in the master makefile, instead of each invidual one?
.SUFFIXES:
# list only those we use
.SUFFIXES: .o .c .cpp .cc .proto .h

# conditionally pass the USE_NIDAQMX to the preprocessor if NADAQMX_PATH file exists, also include the library
ifeq ($(wildcard $(NIDAQMX_PATH)),)
	LDFLAGS := $(LDFLAGS) -lnidaqmx
	CFLAGS := $(CFLAGS) -DUSE_NIDAQMX
endif

# < first item in dependencies list, @ target, ^ dependencies list
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

%.h : %.proto
	protoc $< --cpp_out=$(SRCS)

%.cc : %.proto
	protoc $< --cpp_out=$(SRCS)

# default target is ``all''.  All builds arte_backend only, until I put multiple targets here
# in fact maybe there should only be an 'all' in the master Makefile
all : $(EXE)

$(BUILD_DIR)arte_backend : $(BACKEND_OBJ)
	$(LD)  -o $@  $(LDFLAGS) $(BACKEND_OBJ)



.PHONY : clean

clean :
	rm -f $(BACKEND_OBJ) 
	rm -f $(EXE)
	rm -f arte_pb.pb.h
	rm -f arte_pb.pb.cc
	rm -f *~