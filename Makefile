# Makefile

# 编译器
CC = gcc

# 编译器选项
CFLAGS = -Wall -g -I include

# 目标文件
TARGET = main

# 查找所有源文件
SRCS = $(wildcard src/Tools/*.c src/DataBase/*c src/*.c )

# 对象文件目录
OBJDIR = build

# 对象文件
OBJS = $(SRCS:src/%.c=$(OBJDIR)/%.o)

# 创建对象文件目录及其子目录
$(OBJDIR):
	mkdir -p $(OBJDIR) $(dir $(OBJS))

# 编译目标
$(TARGET): $(OBJDIR) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 编译规则
$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
.PHONY: clean
clean:
	rm -rf $(OBJDIR)