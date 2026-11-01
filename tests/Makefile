# Native tests of the emulator cores, run from this directory (paths are ../-relative).
# Host cc with ASan/UBSan; demo cartridges are generated into ../out/pkg-res.
CC ?= cc
# Glaze's public headers (glaze/*.h), from a sibling glaze-os checkout.
GLAZE_INCLUDE ?= ../../glaze-os/lib/include
$(shell mkdir -p ../out/host)

.DEFAULT_GOAL := check
# Tests that need no external ROM. nes-cpu-test, gameboy-rom-test and
# sega-rom-runners take cartridges you supply (NES_TEST_ROM, GB_TEST_ROM, ...).
check: nes-test gameboy-test mastersystem-test genesis-test sms-banks-test gamegear-test turbografx16-test gamegear-hardware-test pce-hardware-test
.PHONY: check


# NES portable core plus original homebrew cartridge, with memory/UB checking.
NES := $(filter-out ../packages/nes/main.c,$(wildcard ../packages/nes/*.c))
nes-test: ../out/pkg-res/nes/demo.nes
	@mkdir -p ../out/host
	$(CC) -std=gnu11 -O2 -g -fno-strict-aliasing -fsanitize=address,undefined \
	    -o ../out/host/nes-test ../tests/native/nes.c $(NES)
	../out/host/nes-test ../out/pkg-res/nes/demo.nes
.PHONY: nes-test

nes-cpu-test:
	@mkdir -p ../out/host
	$(CC) -std=gnu11 -O2 -g -fno-strict-aliasing -fsanitize=address,undefined \
	    -o ../out/host/nes-trace ../tests/native/nes_trace.c $(NES)
	../out/host/nes-trace "$(NES_TEST_ROM)" "$(NES_TEST_LOG)"
.PHONY: nes-cpu-test

# Game Boy portable adapter and original DMG/CGB cartridges.
GAMEBOY := $(filter-out ../packages/gameboy/main.c ../packages/gameboy/save.c,$(wildcard ../packages/gameboy/*.c))
gameboy-test: gameboy-save-test ../out/pkg-res/gameboy/demo.gb ../out/pkg-res/gameboy/demo.gbc
	@mkdir -p ../out/host
	$(CC) -std=gnu11 -O2 -g -Wall -Wextra -Wno-unused-parameter -fno-strict-aliasing -DIS_LITTLE_ENDIAN -fsanitize=address,undefined \
	    -o ../out/host/gameboy-test ../tests/native/gameboy.c $(GAMEBOY)
	../out/host/gameboy-test ../out/pkg-res/gameboy/demo.gb ../out/pkg-res/gameboy/demo.gbc
.PHONY: gameboy-test

gameboy-rom-test:
	@mkdir -p ../out/host
	$(CC) -std=gnu11 -O2 -g -fno-strict-aliasing -DIS_LITTLE_ENDIAN -fsanitize=address,undefined \
	    -o ../out/host/gameboy-rom ../tests/native/gameboy_rom.c $(GAMEBOY)
	../out/host/gameboy-rom "$(GB_TEST_ROM)"
.PHONY: gameboy-rom-test


gameboy-save-test:
	@mkdir -p ../out/host
	$(CC) -std=gnu11 -O2 -g -Wall -Wextra -I$(GLAZE_INCLUDE) -fsanitize=address,undefined \
	    -o ../out/host/gameboy-save ../tests/native/gameboy_save.c ../packages/gameboy/save.c
	../out/host/gameboy-save
.PHONY: gameboy-save-test


../out/pkg-res/nes/demo.nes: ../tools/mknesdemo.py
	python3 $< --output $@
../out/pkg-res/gameboy/demo.gb: ../tools/mkgbdemo.py
	python3 $< --output-dir $(dir $@) --mode dmg
../out/pkg-res/gameboy/demo.gbc: ../tools/mkgbdemo.py
	python3 $< --output-dir $(dir $@) --mode cgb

# Sega adapters use original cartridges generated only into ignored output.
SMS_CORE := $(filter-out ../packages/mastersystem/main.c ../packages/mastersystem/save.c,$(wildcard ../packages/mastersystem/*.c))
GENESIS_CORE := $(filter-out ../packages/genesis/main.c ../packages/genesis/save.c,$(wildcard ../packages/genesis/*.c))
SMS_FLAGS := -I../third_party/totalsms -I../third_party/totalsms/scheduler -DSMS_PIXEL_WIDTH=16 -DSMS_LITTLE_ENDIAN=1
SEGA_TEST_FLAGS := -std=gnu11 -O2 -g -Wall -Wextra -Wno-unused-parameter -fsanitize=address,undefined -fno-sanitize-recover=all
mastersystem-test: ../out/pkg-res/mastersystem/demo.sms
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) $(SMS_FLAGS) -o ../out/host/sms-test ../tests/native/sega.c $(SMS_CORE)
	../out/host/sms-test $< ../out/host/sms.ppm
genesis-test: ../out/pkg-res/genesis/demo.gen
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) -DTEST_GENESIS -o ../out/host/genesis-test ../tests/native/sega.c $(GENESIS_CORE)
	../out/host/genesis-test $< ../out/host/genesis.ppm
../out/pkg-res/mastersystem/demo.sms: ../tools/mksegademo.py
	python3 $< sms --output $@
../out/pkg-res/genesis/demo.gen: ../tools/mksegademo.py
	python3 $< genesis --output $@
.PHONY: mastersystem-test genesis-test

sms-banks-test:
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) $(SMS_FLAGS) -o ../out/host/sms-banks ../tests/native/sms_banks.c $(SMS_CORE) ../tests/native/sega_audio_stub.c
	../out/host/sms-banks
.PHONY: sms-banks-test

sega-rom-runners:
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) $(SMS_FLAGS) -o ../out/host/sms-rom ../tests/native/sega_rom.c $(SMS_CORE)
	$(CC) $(SEGA_TEST_FLAGS) -o ../out/host/genesis-rom ../tests/native/sega_rom.c $(GENESIS_CORE)
.PHONY: sega-rom-runners

GG_CORE := $(filter-out ../packages/gamegear/main.c ../packages/gamegear/save.c,$(wildcard ../packages/gamegear/*.c))
PCE_CORE := $(filter-out ../packages/turbografx16/main.c ../packages/turbografx16/save.c,$(wildcard ../packages/turbografx16/*.c))
gamegear-test: ../out/pkg-res/gamegear/demo.gg
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) $(SMS_FLAGS) -DTEST_GG -o ../out/host/gamegear-test ../tests/native/console.c $(GG_CORE)
	../out/host/gamegear-test $< ../out/host/gamegear.ppm
turbografx16-test: ../out/pkg-res/turbografx16/demo.pce
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) -fno-strict-aliasing -o ../out/host/turbografx16-test ../tests/native/console.c $(PCE_CORE)
	../out/host/turbografx16-test $< ../out/host/turbografx16.ppm
../out/pkg-res/gamegear/demo.gg: ../tools/mksegademo.py
	python3 $< gamegear --output $@
../out/pkg-res/turbografx16/demo.pce: ../tools/mkpcedemo.py
	python3 $< --output $@
.PHONY: gamegear-test turbografx16-test

gamegear-hardware-test: ../out/pkg-res/gamegear/demo.gg
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) $(SMS_FLAGS) -o ../out/host/gamegear-hardware ../tests/native/gamegear_hardware.c $(GG_CORE) ../tests/native/sega_audio_stub.c
	../out/host/gamegear-hardware $<
pce-hardware-test:
	@mkdir -p ../out/host
	$(CC) $(SEGA_TEST_FLAGS) -fno-strict-aliasing -o ../out/host/pce-hardware ../tests/native/pce_hardware.c $(PCE_CORE)
	../out/host/pce-hardware
.PHONY: gamegear-hardware-test pce-hardware-test
