# An SDK project. The top-level Makefile packs it with its icon and generated demo.
GLAZE_SDK ?= $(abspath ../../../glaze-os/out/sdk/glaze-sdk)
SOURCES := $(wildcard *.c)
CPPFLAGS += -DNDEBUG -I../../third_party/clownmdemu/platform -Wno-unused-parameter
include $(GLAZE_SDK)/share/app.mk
