# Makefile

# 编译器
CC = gcc

# 编译器选项
CFLAGS = -Wall -g -I include

# 目标文件
TARGET = main

# 查找所有源文件
SRCS = $(wildcard src/Tools/*.c src/DataBase/*.c src/*.c)

# 对象文件目录
OBJDIR = build

# 对象文件
OBJS = $(SRCS:src/%.c=$(OBJDIR)/%.o)

# 测试文件
TEST_SRCS = $(wildcard test/*.c)
TEST_OBJS = $(TEST_SRCS:test/%.c=$(OBJDIR)/test_%.o)
TEST_TARGETS = $(TEST_SRCS:test/%.c=$(OBJDIR)/test_%)

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

# 编译测试文件
$(OBJDIR)/test_%.o: test/%.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接测试文件
$(OBJDIR)/test_%: $(OBJDIR)/test_%.o $(filter-out $(OBJDIR)/main.o, $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^

# 运行所有测试
.PHONY: test
test: $(TEST_TARGETS)
	for test in $(TEST_TARGETS); do ./$$test; done

# 清理
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) $(TEST_TARGETS)