CSRCS += $(shell find -L $(LVGL_DIR)/app/ui -name "*.c")

# 添加头文件路径
CFLAGS += -I$(LVGL_DIR)/app/ui

# 添加库路径
# LDFLAGS += -L$(LVGL_DIR)/path/to/libs

# # 库文件链接
# LDLIBS += -l<library_name>


