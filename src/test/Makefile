CC = g++44
LD = g++44

LDFLAGS = -pthread -ldl -D__STDC_LIMIT_MACROS -lprotobuf -lboost_system
CFLAGS = -pthread -std=c++0x -I ../ -I /usr/local/include/ -I /usr/local/ -I /usr/local/boost -D__STDC_LIMIT_MACROS

GLOM_OBJ = ../glom.o test_glom.o ../arte_pb.pb.o ../arte_command.pb.o
CTR05TIMER_OBJ = ./test_ctr05_timer.o ../a_timer.o ../ctr05_timer.o ../arte_pb.pb.o ../arte_command.pb.o
SINKSOURCE_OBJ = ../a_data_sink.o ../a_data_source.o test_source_sink.o ../arte_pb.pb.o ../arte_command.pb.o ../glom.o ../global_state.o

.SUFFIXES:
# list only those we use
.SUFFIXES: .o .c .cpp .cc .proto

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

test_glom : $(GLOM_OBJ)
	$(LD) -o $@ $(LDFLAGS) $(GLOM_OBJ)

test_ctr05_timer : $(CTR05TIMER_OBJ)
	$(LD) $(CTR05TIMER_OBJ) $(LDFLAGS) -o $@

test_sink_source : $(SINKSOURCE_OBJ)
	$(LD) $(SINKSOURCE_OBJ) -lnidaqmx $(LDFLAGS) -o $@

.PHONY : clean

clean :
	rm ../a_timer.o
	rm ../ctr05_timer.o
	rm ../system_clock_timer.o
	rm ./test_ctr05_timer.o