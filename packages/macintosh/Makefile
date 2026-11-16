GLAZE_SDK ?= $(abspath ../../../glaze-os/out/sdk/glaze-sdk)
BUILD ?= build
CORE := ../../third_party/umac
GEN := $(abspath $(BUILD)/generated)
HOSTCC ?= cc
SOURCES := main.c adapter.c view.c $(wildcard vendor_*.c) $(GEN)/m68kops.c
CPPFLAGS += -I$(CORE)/include -I$(CORE)/Musashi -I$(GEN) -Icompat -DMUSASHI_CNF='"../include/m68kconf.h"' -DUMAC_MEMSIZE=512 -DDISP_WIDTH=512 -DDISP_HEIGHT=342 -DENABLE_DASM=0 -fno-strict-aliasing -Wno-unused-parameter
$(GEN)/m68kmake: $(CORE)/Musashi/m68kmake.c
	@mkdir -p $(GEN)
	$(HOSTCC) -O2 $< -o $@
$(GEN)/m68kops.c $(GEN)/m68kops.h &: $(GEN)/m68kmake $(CORE)/Musashi/m68k_in.c
	$(GEN)/m68kmake $(GEN) $(CORE)/Musashi/m68k_in.c
include $(GLAZE_SDK)/share/app.mk
$(SDK_OBJECTS): $(GEN)/m68kops.h
.DEFAULT_GOAL := all
