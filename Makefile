CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = fsh  
LIBS = -lreadline

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

.PHONY: all clean
