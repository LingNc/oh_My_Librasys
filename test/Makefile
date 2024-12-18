# Makefile for tests

# 编译器
CC = gcc

# 编译器选项
CFLAGS = -Wall -Wextra -g -I ../include
LDFLAGS = -lm

# 查找所有源文件
SRC_DIR = ../src
OBJ_DIR = ../build

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# 排除包含 main 函数的文件
OBJ_FILES := $(filter-out $(OBJ_DIR)/main.o, $(OBJ_FILES))

TEST_FILES = $(wildcard *.c)
TEST_EXECUTABLES = $(patsubst %.c, %, $(TEST_FILES))

all: $(TEST_EXECUTABLES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $< $(OBJ_FILES) -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TEST_EXECUTABLES)

.PHONY: all clean