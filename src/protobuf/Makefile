CC = gcc
CFLAGS = -O0 -g3 -Wall -std=c99
LFLAGS = 
DEPS = protobuf.h
OBJS = protobuf.o varint.o

all: protobuf_reader
	cd test; make all;
	
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

protobuf_reader: $(OBJS) main.o
	$(CC) -o $@ $^ 

test_protobuf: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -f *.o
	rm -f protobuf_reader;
	cd test; make clean;
	
rebuild: clean all
