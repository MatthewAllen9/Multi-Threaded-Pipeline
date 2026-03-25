CC      = gcc
CFLAGS  = -Wall -Wextra -pthread
LDFLAGS = -pthread

TARGET  = pipeline

SRC     = pipeline.c

.PHONY: all clean

all: CFLAGS += -O0
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)