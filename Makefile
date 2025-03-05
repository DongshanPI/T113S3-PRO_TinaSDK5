# Makefile
.PHONY: all build_and_analyze build analyze clean

# Tina SDK Path Config
TINA_SDK_ROOT = /home/wangxiankang/Desktop/t113s3/100ask_t113s3pro_v1.3/tina5sdk-bsp

CC = ${TINA_SDK_ROOT}/out/t113/evb1_auto/buildroot/buildroot/host/bin/arm-linux-gnueabi-gcc

# LVGL 相关目录
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= $(shell pwd)

# 编译选项
CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ -Wall -Wshadow -Wundef -Wmissing-prototypes \
          -Wno-discarded-qualifiers -Wextra -Wno-unused-function \
          -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing \
          -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized \
          -Wno-unused-parameter -Wno-missing-field-initializers \
          -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral \
          -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
          -Wreturn-type -Wmultichar -Wformat-security \
          -Wno-ignored-qualifiers -Wno-sign-compare \
          -Wno-error=missing-prototypes -Wdouble-promotion \
          -Wclobbered -Wdeprecated -Wempty-body -Wstack-usage=2048 \
          -Wno-unused-value

# 可执行文件名称
BIN = cattv

# 定义对象文件输出目录
OBJDIR = build

# 包含lvgl Makefile
include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/resource/build.mk
include $(LVGL_DIR)/app/ui/build.mk
include $(LVGL_DIR)/app/tplayer/build.mk
include $(LVGL_DIR)/app/data/build.mk

# main.c
CSRCS += main.c

# 对象文件扩展名
OBJEXT ?= .o

# 当前目录
PWD := $(shell pwd)

# 将绝对路径转换为相对路径（相对于 Makefile 目录）
relative_ASRCS = $(patsubst $(PWD)/%, %, $(ASRCS))
relative_CSRCS = $(patsubst $(PWD)/%, %, $(CSRCS))

# 定义对象文件路径，保持目录结构
AOBJS = $(patsubst %.S, $(OBJDIR)/%.o, $(relative_ASRCS))
COBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(relative_CSRCS))

# 收集所有源文件
SRCS = $(ASRCS) $(CSRCS)
OBJS = $(AOBJS) $(COBJS)

# 默认目标：构建并分析
all: build_and_analyze

# 构建并分析目标
build_and_analyze:
	@echo "Starting build..."
	@start_time=$$(date +%s); \
	$(MAKE) build; \
	build_status=$$?; \
	if [ $$build_status -ne 0 ]; then \
		echo "Build failed."; \
		exit $$build_status; \
	fi; \
	end_time=$$(date +%s); \
	elapsed=$$((end_time - start_time)); \
	echo "Build successful. Time taken: $$elapsed seconds."; \
	echo "=== Executable Size Analysis ==="; \
	size $(BIN) | awk 'NR==2 {printf "Code (.text): %s bytes\nData (.data): %s bytes\nBSS (.bss): %s bytes\nTotal: %s bytes\n", $$1, $$2, $$3, $$1 + $$2 + $$3}'; \
	echo "=============================="

# 构建目标：编译和链接
build: $(BIN)

# 编译规则：将 .c 和 .S 文件编译为 OBJDIR 中的 .o 文件
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling $<"

$(OBJDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Assembling $<"


# 链接目标：将所有对象文件链接为最终可执行文件
$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(LDFLAGS)
	@echo "Linking to create executable $(BIN)"

# 清理目标：删除生成的文件和目录
clean: 
	@echo "Cleaning up..."
	@rm -f $(BIN)
	@rm -f $(OBJS)
	@rm -rf $(OBJDIR)
	@echo "Cleaned successfully."