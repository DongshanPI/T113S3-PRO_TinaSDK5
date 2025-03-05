# 获取源文件
CSRCS += $(shell find -L $(LVGL_DIR)/app/tplayer -name "*.c")

TINA_LIB_DIR := ${TINA_SDK_ROOT}/out/t113/evb1_auto/buildroot/buildroot/host/arm-buildroot-linux-gnueabi/sysroot

# 添加头文件路径
CFLAGS += -I$(LVGL_DIR)/app/tplayer \
          -I$(TINA_LIB_DIR)/usr/include/ \
          -I$(TINA_LIB_DIR)/usr/include/libcedarx/ \
          -I$(TINA_LIB_DIR)/usr/include/libcedarc/ 
          
          
# 添加库路径
LDFLAGS += -L$(TINA_LIB_DIR)/lib \
           -L$(TINA_LIB_DIR)/usr/lib \
           -Wl,-rpath=$(TINA_LIB_DIR)/usr/lib \
           -ltplayer -lxplayer -lvdecoder -ladecoder -lsubdecoder \
           -lcdx_parser -lcdx_playback -lcdx_stream -lcdx_base \
           -luapi -lz -lasound -lstdc++ -lpthread -lrt -ldl -lm \
           -lcdc_base -lVE -lMemAdapter -lssl -lcrypto
           