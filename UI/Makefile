# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -Wextra -g -I UI_include -I ../../include

# 链接选项
LDFLAGS = -lncursesw -lmenuw -lform 

# 目标文件
TARGET = main

# 源文件
SRCS = $(wildcard ../../src/Tools/*.c ../../src/DataBase/*.c ../../src/*.c *.c ../../src/Book.c)

# 生成的对象文件
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 编译源文件为对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标
.PHONY: all clean