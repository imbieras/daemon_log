GFLAGS := -Wall -Wextra -pedantic -std=gnu99 -O2
CFLAGS := $(GFLAGS) -ggdb3 -g
SOURCES := $(wildcard *.c)
OBJS := $(SOURCES:.c=.o)
BIN := daemon_log
LDFLAGS := -llink_core -lutils_modules -lplatform_port -lmiddleware_implementation -largp -lblobmsg_json -lubox -lubus -llua

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)
