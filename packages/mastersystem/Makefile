# An SDK project. The top-level Makefile packs it with its icon and generated demo.
GLAZE_SDK ?= $(abspath ../../../glaze-os/out/sdk/glaze-sdk)
SOURCES := $(wildcard *.c)
CPPFLAGS += -I../../third_party/totalsms -I../../third_party/totalsms/scheduler -DSMS_PIXEL_WIDTH=16 -DSMS_LITTLE_ENDIAN=1 -Wno-unused-parameter
include $(GLAZE_SDK)/share/app.mk
