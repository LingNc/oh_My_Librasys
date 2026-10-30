CC ?= gcc

# UI 后端: both(默认) | cli | tui
#   cli  = 只编译命令行界面，不依赖 ncursesw
#   tui  = 只编译 TUI（保留 CLI 入口作为回退）
#   both = 两者都编译（默认）
UI ?= both

CPPFLAGS ?= -Iinclude
CFLAGS ?= -std=gnu11 -Wall -Wextra -Wno-error=incompatible-pointer-types -g
LDFLAGS ?=
LDLIBS ?=

NCURSES_CFLAGS := $(shell pkg-config --cflags ncursesw 2>/dev/null)
NCURSES_LIBS   := $(shell pkg-config --libs ncursesw 2>/dev/null)

TARGET := main
BUILD_DIR := build

ALL_SRC_FILES := $(shell find src -type f -name '*.c' | sort)

ifeq ($(UI),cli)
SRC_FILES := $(filter-out src/ui/tui/%,$(ALL_SRC_FILES))
else
SRC_FILES := $(ALL_SRC_FILES)
CPPFLAGS  += $(NCURSES_CFLAGS) -DHAVE_TUI=1
LDLIBS    += $(NCURSES_LIBS)
endif

APP_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
LIB_OBJECTS := $(filter-out $(BUILD_DIR)/src/main.o,$(APP_OBJECTS))
TEST_SUPPORT_OBJECTS := $(filter-out $(BUILD_DIR)/src/main.o \
	$(BUILD_DIR)/src/function.o $(BUILD_DIR)/src/DataBase/% \
	$(BUILD_DIR)/src/ui/%,$(APP_OBJECTS))

UNIT_TEST_SOURCES := $(filter-out test/test_check.c test/test_tui_width.c,$(wildcard test/*.c))
UNIT_TEST_TARGETS := $(patsubst test/%.c,$(BUILD_DIR)/test_%,$(UNIT_TEST_SOURCES))

# 纯逻辑的 TUI 测试：只链接 width.o（该文件不依赖 ncurses），
# 因此即使 make UI=cli 也能跑。
TUI_WIDTH_OBJECT := $(BUILD_DIR)/src/ui/tui/width.o
TUI_WIDTH_TEST   := $(BUILD_DIR)/test_tui_width
UNIT_TEST_TARGETS += $(TUI_WIDTH_TEST)

$(TUI_WIDTH_TEST): test/test_tui_width.c $(TUI_WIDTH_OBJECT)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

CHECK_TEST := $(BUILD_DIR)/test_check

.DEFAULT_GOAL := all

.PHONY: all
all: $(TARGET)

$(TARGET): $(APP_OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/test_%: test/%.c $(LIB_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(TEST_SUPPORT_OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

.PHONY: test
test: $(UNIT_TEST_TARGETS)
	@set -e; for test in $(UNIT_TEST_TARGETS); do ./$$test; done

# 只跑 TUI 的纯逻辑测试（不依赖 ncurses，也不受 test_vector 已知问题影响）
.PHONY: test-tui
test-tui: $(TUI_WIDTH_TEST)
	./$(TUI_WIDTH_TEST)

.PHONY: check-test
check-test: $(CHECK_TEST)
	./$(CHECK_TEST)

$(CHECK_TEST): test/test_check.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(shell pkg-config --libs check 2>/dev/null) -o $@

.PHONY: preprocess
preprocess: $(SRC_FILES)
	@mkdir -p preprocess
	@set -e; for src in $^; do \
		$(CC) $(CPPFLAGS) $(CFLAGS) -E "$$src" \
			-o "preprocess/$$(basename "$$src" .c).i"; \
	done

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET) preprocess

-include $(APP_OBJECTS:.o=.d)
