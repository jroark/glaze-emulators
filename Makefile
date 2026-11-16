# Glaze emulator packages, built against the Glaze application SDK.
#
#   make                  out/pkg/<emulator>.gzp for every emulator
#   make test             native core tests (host cc, ASan/UBSan; no SDK needed)
#
# GLAZE_SDK defaults to the SDK exported by a sibling glaze-os checkout
# (`make sdk` there). Each packages/<emulator> is an SDK project; this file
# adds the generated demo cartridges and the icon, which live outside res/.
GLAZE_SDK ?= $(abspath ../glaze-os/out/sdk/glaze-sdk)
CROSS ?= mipsel-linux-gnu-
PYTHON ?= python3
OUT ?= out
EMULATORS := nes gameboy mastersystem gamegear genesis turbografx16 macintosh

.DEFAULT_GOAL := all
.PHONY: all test clean FORCE sdk-check
all: $(foreach e,$(EMULATORS),$(OUT)/pkg/$(e).gzp)

sdk-check:
	@test -f "$(GLAZE_SDK)/share/app.mk" || { echo "GLAZE_SDK=$(GLAZE_SDK) is not a Glaze SDK;" \
	    "run 'make sdk' in ../glaze-os or set GLAZE_SDK" >&2; exit 1; }

# The SDK's app.mk tracks sources and headers, so always ask it.
$(OUT)/build/%/app: FORCE | sdk-check
	$(MAKE) -C packages/$* GLAZE_SDK=$(GLAZE_SDK) CROSS=$(CROSS) BUILD=$(abspath $(OUT)/build/$*) $(abspath $@)

# ROM binaries are generated into ignored build output, never source directories.
GENERATED_nes := $(OUT)/pkg-res/nes/demo.nes
GENERATED_gameboy := $(OUT)/pkg-res/gameboy/demo.gb $(OUT)/pkg-res/gameboy/demo.gbc
GENERATED_mastersystem := $(OUT)/pkg-res/mastersystem/demo.sms
GENERATED_gamegear := $(OUT)/pkg-res/gamegear/demo.gg
GENERATED_genesis := $(OUT)/pkg-res/genesis/demo.gen
GENERATED_turbografx16 := $(OUT)/pkg-res/turbografx16/demo.pce
RES = $(wildcard packages/$(1)/res/*) $(GENERATED_$(1))

$(OUT)/pkg-res/nes/demo.nes: tools/mknesdemo.py
	$(PYTHON) $< --output $@
$(OUT)/pkg-res/gameboy/demo.gb: tools/mkgbdemo.py
	$(PYTHON) $< --output-dir $(dir $@) --mode dmg
$(OUT)/pkg-res/gameboy/demo.gbc: tools/mkgbdemo.py
	$(PYTHON) $< --output-dir $(dir $@) --mode cgb
$(OUT)/pkg-res/mastersystem/demo.sms: tools/mksegademo.py
	$(PYTHON) $< sms --output $@
$(OUT)/pkg-res/gamegear/demo.gg: tools/mksegademo.py
	$(PYTHON) $< gamegear --output $@
$(OUT)/pkg-res/genesis/demo.gen: tools/mksegademo.py
	$(PYTHON) $< genesis --output $@
$(OUT)/pkg-res/turbografx16/demo.pce: tools/mkpcedemo.py
	$(PYTHON) $< --output $@

.SECONDEXPANSION:
$(OUT)/pkg/%.gzp: $(OUT)/build/%/app packages/%/Info packages/%/icon.gzi $$(call RES,$$*)
	@mkdir -p $(dir $@)
	$(PYTHON) $(GLAZE_SDK)/tools/mkromfs.py --package $@ Info=packages/$*/Info app=$< \
	    res/icon.gzi=packages/$*/icon.gzi $(foreach f,$(call RES,$*),res/$(notdir $(f))=$(f))
	$(PYTHON) $(GLAZE_SDK)/bin/glaze-sdk check $@

test:
	$(MAKE) -C tests check

clean:
	rm -rf $(OUT)
