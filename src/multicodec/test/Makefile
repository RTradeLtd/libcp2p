CC = gcc
CFLAGS = -O0
DEPS = ../multicodec/multicodec.h
OBJS = test_multicodec.o ../multicodec/multicodec.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_multicodec: $(OBJS)

	$(CC) -o $@ $(OBJS)

all: test_multicodec

clean:
	rm -f test_multicodec;
	rm -f test_multicodec.o;