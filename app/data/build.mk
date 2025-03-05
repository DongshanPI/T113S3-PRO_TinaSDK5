CSRCS += $(shell find -L $(LVGL_DIR)/app/data -name "*.c")

CFLAGS += -I$(LVGL_DIR)/app/data


