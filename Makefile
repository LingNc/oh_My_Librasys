# Makefile

# 编译器
CC = gcc

# 编译器选项
CFLAGS = -Wall -Wextra -g -I include

# 链接选项
LDFLAGS =

# 目标文件
TARGET = main

# 查找所有源文件
SRCS = $(wildcard src/models/*c src/Tools/*.c src/DataBase/*.c src/ui/components/*.c src/ui/*.c src/*.c)

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
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 编译源文件为对象文件
$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 编译测试文件为对象文件
$(OBJDIR)/test_%.o: test/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接测试文件
$(OBJDIR)/test_%: $(OBJDIR)/test_%.o $(filter-out $(OBJDIR)/main.o, $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 运行所有测试
.PHONY: test
test: $(TEST_TARGETS)
	for test in $(TEST_TARGETS); do ./$$test; done

# 清理生成的文件
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) $(TEST_TARGETS)

# 预处理器输出
.PHONY: preprocess
preprocess: $(SRCS)
	@mkdir -p preprocess
	@for src in $^; do \
		$(CC) $(CFLAGS) -E $$src -o preprocess/`basename $$src .c`.i; \
	done