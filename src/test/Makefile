CC = gcc
LD = $(CC)

LDFLAGS = -ldl -lprotobuf -pthread
CFLAGS = -I ../

GLOM_OBJ = ../glom.o test_glom.o ../arte_pb.pb.o ../arte_command.pb.o
GLOM_EXE = test_glom

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

test_glom : $(GLOM_OBJ)
	$(LD) -o $@ $(LDFLAGS) $(GLOM_OBJ)