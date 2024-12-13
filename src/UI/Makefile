# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -Wextra -g

# 链接选项
LDFLAGS = -lncurses -lmenu

# 目标文件
TARGET = menu

# 源文件
SRCS = menu.c

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