GFLAGS := -Wall -Wextra -pedantic -std=c99 -O2
CFLAGS := $(GFLAGS)
SOURCES := $(wildcard *.c)
OBJS := $(SOURCES:.c=.o)
BIN := ../daemon_log
LDFLAGS := -L../tuya-iot-core-sdk/build/lib -llink_core -lutils_modules -lplatform_port -lmiddleware_implementation 
INCLUDE := -I../tuya-iot-core-sdk/utils -I../tuya-iot-core-sdk/include -I../tuya-iot-core-sdk/interface 

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)
