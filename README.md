# Glaze emulators

Console emulator apps for [Glaze](https://github.com/jroark/glaze-os), the
microkernel OS for the Casio BE-300. Each is an installable `.gzp` package:

| Package | System | Core | Guide |
| --- | --- | --- | --- |
| `macintosh` | Macintosh 512K | uMac / Musashi | [Macintosh](docs/MACINTOSH.md) |
| `nes` | NES / Famicom (iNES and NES 2.0, mappers 0–4) | InfoNES | [NES](docs/NES.md) |
| `gameboy` | Game Boy and Game Boy Color | gnuboy | [Game Boy](docs/GAMEBOY.md) |
| `mastersystem` | Sega Master System | TotalSMS | [Sega](docs/SEGA.md) |
| `genesis` | Sega Genesis / Mega Drive | ClownMDEmu | [Sega](docs/SEGA.md) |
| `gamegear` | Sega Game Gear | TotalSMS | [Game Gear](docs/GAMEGEAR.md) |
| `turbografx16` | TurboGrafx-16 / PC Engine HuCards | Beetle PCE Fast | [TurboGrafx-16](docs/TURBOGRAFX16.md) |

No ROMs are included. The console packages carry original demo cartridges,
generated at build time by the scripts in `tools/`. Bring your own cartridges
on a CompactFlash card.

## Building

The packages build against the Glaze application SDK, which a sibling
[glaze-os](https://github.com/jroark/glaze-os) checkout exports:

```sh
git clone https://github.com/jroark/glaze-os ../glaze-os
(cd ../glaze-os && ./build.sh sdk)    # out/sdk/glaze-sdk
./build.sh                            # out/pkg/<emulator>.gzp, in the mips-cross-dev container
make                                  # the same, with mipsel-linux-gnu-gcc installed
```

Set `GLAZE_SDK` to use another SDK, or `GLAZE_DIR` (for `build.sh` and the smoke
tests) to use another glaze-os checkout. Each `packages/<emulator>` is an SDK
project. The top-level Makefile adds its icon and generated demo cartridge,
packs it with the SDK's `mkromfs.py`, and checks it with `glaze-sdk check`.

Install a package by putting it on a card image and opening it in Files:

```sh
../glaze-os/tools/mkcfimg.sh out/cf.img 16 16 out/pkg/*.gzp
be300 --nand ../glaze-os/out/glaze.bin --rtc-host-time --cf out/cf.img
```

## Testing

```sh
make test                                   # native core tests (host cc, ASan/UBSan)
python3 tests/nes_smoke.py --be300 /path/to/be300
python3 tests/gameboy_smoke.py --be300 /path/to/be300
python3 tests/sega_smoke.py --be300 /path/to/be300 [--system new]
```

`make test` needs only a host C compiler and glaze-os's public headers
(`GLAZE_INCLUDE`, default `../glaze-os/lib/include`). The smoke tests build the
packages here and a scripted OS image in the glaze-os checkout, install and
play each emulator in the BE-300 emulator, then restore a normal image. Run
them where the native cross toolchain is available.

## License

Glaze code is GPLv3 (`LICENSE`). The cores keep their own licenses, listed
in [third_party/README.md](third_party/README.md). ClownMDEmu is
AGPL-3.0-or-later, so the Genesis package is too.
