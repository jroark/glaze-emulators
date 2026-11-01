# CLAUDE.md

Console emulator packages for Glaze (the Casio BE-300 OS in the sibling
`../glaze-os` repo), built against its application SDK. See README.md for the
package list and commands.

- **Build:** `./build.sh` (Docker, `mips-cross-dev`, mounting `../glaze-os`) or `make`
  with `mipsel-linux-gnu-gcc`. Run `make sdk` in glaze-os first; `GLAZE_SDK` and
  `GLAZE_DIR` override the sibling paths. Builds must stay warning-free.
- **Test:** `make test` runs the native core tests (host cc with ASan/UBSan;
  `tests/Makefile`, run from `tests/` with `../`-relative paths). `tests/*_smoke.py`
  boot the emulator with a scripted OS image built in glaze-os.
- **Layout:** `packages/<emulator>` (an SDK project: sources, `Info`, `icon.gzi`,
  `res/`), shared Sega/PCE frontend code in `lib/sega` (included by the packages
  as `../../lib/sega/*.c`), cores in `third_party/` with pins and port changes in
  each `UPSTREAM.md`, demo cartridge generators in `tools/`.
- **Platform:** freestanding MIPS II, soft float, Glaze's minimal libc and libui;
  integer-only audio and video. The SDK's headers (`glaze/*.h`) are the
  only OS interface. Never link libgcc or a Linux libc.
- **Never commit ROMs** (`.gitignore` blocks the usual extensions), including
  generated demos. Demos are generated into `out/pkg-res/` by the build. Keep
  test cartridges outside the repo or under ignored `out/`.
- Keep cores' licences (GPL-2.0-or-later elected GPLv3, Apache-2.0, MIT,
  AGPL-3.0-or-later for ClownMDEmu) and record any core change in its `UPSTREAM.md`.
