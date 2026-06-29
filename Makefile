CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = ping

all: $(TARGET)

$(TARGET): ping.c
	$(CC) $(CFLAGS) -o $(TARGET) ping.c

clean:
	rm -f $(TARGET)
