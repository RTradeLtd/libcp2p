DEBUG=true

CC = gcc
RM = rm -f
CFLAGS = -fPIC -O0 -std=c99 \
	-Wall -Wextra -pedantic -Werror \
	-Wdeclaration-after-statement \
	-Wno-format-zero-length \
	-Wold-style-definition \
	-Woverflow \
	-Wpointer-arith \
	-Wunused \
	-Wvla

ifdef DEBUG
CFLAGS += -g3
endif

LDFLAGS = -g
LDLIBS =

TARGET_LIB = libmultihash.a
TARGET_BIN = multihash

SRCS = src/hashes.c src/errors.c src/multihash.c
OBJS = $(SRCS:.c=.o)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -I include $< -o $@

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	ar rcs $@ $^

# Tests

TEST_SRCS = $(wildcard tests/c/test_*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_BINS = $(TEST_OBJS:.o=)

PHONY += test
test: $(TEST_BINS)
	@for t in $^; do                                   \
	  echo;                                            \
	  echo '***' "$$t.c" '***';                        \
	  "./$$t" || ERROR=1;                              \
	done;                                              \
	echo;                                              \
	exit $$ERROR

tests/c/test_%.o: tests/c/test_%.c tests/c/minunit.h
	$(CC) $(CFLAGS) -c -I include $< -o $@

tests/c/test_%: tests/c/test_%.o $(TARGET_LIB)
	$(CC) $(LDFLAGS) $^ -o $@

# Utils

PHONY += clean dist-clean

clean:
	$(RM) $(OBJS)
	$(RM) $(TEST_BINS)
	$(RM) $(TARGET_LIB)
	$(RM) $(TARGET_BIN)
	$(RM) $(MAIN_O)

dist-clean: clean

PHONY += build
build: all

.PHONY: $(PHONY)
