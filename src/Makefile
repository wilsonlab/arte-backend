# this file gets included by the one in the main project directory,
# so act as though we are there, not in src/
CC = gcc
LD = $(CC)
# in next line: don't nesceccarily do -lnidaqmx.  Instead, check for it and conditional link, as in
# this post (http://stackoverflow.com/questions/3910054/makefile-conditional-include)
LDFLAGS = -ldl -lnidaqmx -lprotobuf
BUILD_DIR = build/

SRCS = src/
BACKEND_SRC = $(SRCS)arte.cpp $(SRCS)filt.cpp $(SRCS)neural_daq.cpp $(SRCS)trode.cpp $(SRCS)util.cpp $(SRCS)lfp_bank.cpp $(SRCS)filtered_buffer.cpp $(SRCS)filter_fun.cpp $(SRCS)timer.cpp $(SRCS)process_command.cpp $(SRCS)netcom.cpp $(SRCS)datapacket.cpp $(SRCS)arte_pb.pb.cc $(SRCS)arte_command.pb.cc $(SRCS)apteopt.cpp

BACKEND_OBJ = $(SRCS)process_command.o $(SRCS)arteopt.o  $(SRCS)arte_pb.pb.o $(SRCS)arte_command.pb.o $(SRCS)filt.o $(SRCS)neural_daq.o $(SRCS)trode.o $(SRCS)util.o $(SRCS)lfp_bank.o $(SRCS)filtered_buffer.o $(SRCS)filter_fun.o $(SRCS)timer.o  $(SRCS)netcom.o $(SRCS)datapacket.o $(SRCS)arte.o

CFLAGS = -pthread -I /usr/local/boost_1_45_0/ -I $(SRCS) -D__STDC_LIMIT_MACROS -O3

OBJS = src/
RM = rm
EXE = $(BUILD_DIR)arte_backend

# clear out all suffixes (from http://owen.sj.ca.us/~rk/howto/slides/make/slides/makesuff.html)
# should this and the .c.o: rule be in the master makefile, instead of each invidual one?
.SUFFIXES:
# list only those we use
.SUFFIXES: .o .c .cpp .cc .proto

# define a suffix rule for .cpp -> .o, .cc -> .o, .proto -> .cc
# reminder, < is the first item in the dependencies list
#           and @ is the left side of  target:dep; ^ is the right side
%.o : %.cpp
	$(CC) $(CFLAGS) -I /usr/local/boost_1_45_0/ -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -I /usr/local/boost_1_45_0/ -c $< -o $@
%.o : %.cc
	$(CC) $(CFLAGS) -I /usr/local/boost_1_45_0/ -c $< -o $@

%.cc : %.proto
	$protoc $< --cpp_out=$(SRCS)

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