# this file gets included by the one in the main project directory,
# so act as though we are there, not in src/
CC = g++44
LD = $(CC)
# in next line: don't nesceccarily do -lnidaqmx.  Instead, check for it and conditional link, as in
# this post (http://stackoverflow.com/questions/3910054/makefile-conditional-include)
LDFLAGS = -std=c++0x -ldl -lnidaqmx -lprotobuf -lzmq
BUILD_DIR = build/
NIDAQMX_PATH = /usr/local/include/NIDAQmx.h
SRCS = src/

BACKEND_SRC =  $(SRCS)arte.cpp $(SRCS)filt.cpp $(SRCS)neural_daq.cpp $(SRCS)trode.cpp $(SRCS)util.cpp 
BACKEND_SRC += $(SRCS)lfp_bank.cpp $(SRCS)filtered_buffer.cpp $(SRCS)filter_fun.cpp $(SRCS)timer.cpp 
BACKEND_SRC += $(SRCS)process_command.cpp $(SRCS)netcom.cpp $(SRCS)arte_pb.pb.cc 
BACKEND_SRC += $(SRCS)arte_command.pb.cc $(SRCS)arteopt.cpp $(SRCS)glom.cpp 
BACKEND_SRC += $(SRCS)arte_command_port.cpp $(SRCS)nidaq_data_source.cpp $(SRCS)a_data_source.cpp
BACKEND_SRC += $(SRCS)a_timer.cpp $(SRCS)global_state.cpp $(SRCS)datapacket.cpp

BACKEND_OBJ =  $(SRCS)arte.o $(SRCS)filt.o $(SRCS)neural_daq.o $(SRCS)trode.o $(SRCS)util.o 
BACKEND_OBJ += $(SRCS)lfp_bank.o $(SRCS)filtered_buffer.o $(SRCS)filter_fun.o $(SRCS)timer.o 
BACKEND_OBJ += $(SRCS)process_command.o $(SRCS)netcom.o $(SRCS)arte_pb.pb.o 
BACKEND_OBJ += $(SRCS)arte_command.pb.o $(SRCS)arteopt.o $(SRCS)glom.o
BACKEND_OBJ += $(SRCS)arte_command_port.o $(SRCS)nidaq_data_source.o $(SRCS)a_data_source.o
BACKEND_OBJ += $(SRCS)a_timer.o $(SRCS)global_state.o $(SRCS)datapacket.o 

# path to boost library added to 
CFLAGS = -D__STDC_LIMIT_MACROS -pthread -std=c++0x -O3 -I /usr/local/include/ -I $(wildcard /usr/local/boost_1_45_0) -I $(wildcard /usr/local/boost-trunk) -I $(wildcard /usr/local/boost_1_45_0/boost/property_tree) -I $(SRCS)


OBJS = src/
RM = rm
EXE = $(BUILD_DIR)arte_backend3

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

#%.o : %.h
#	$(CC) $(CFLAGS) -c $< -o $@

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

$(EXE) : $(BACKEND_OBJ)
	$(LD)  -D__STDC_LIMIT_MACROS -o $@  $(BACKEND_OBJ) $(LDFLAGS)



.PHONY : clean

clean :
	rm -f $(BACKEND_OBJ) 
	rm -f $(EXE)
	rm -f arte_pb.pb.h
	rm -f arte_pb.pb.cc
	rm -f *~