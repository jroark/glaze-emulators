# Third-party code

Vendored emulator cores. Each directory's `UPSTREAM.md` records its pin and the
port changes made for Glaze; the packages under `packages/` hold the Glaze glue.

| Directory | Project | Version | License |
| --- | --- | --- | --- |
| `beetle-pce-fast/` | [Beetle PCE Fast](https://github.com/libretro/beetle-pce-fast-libretro), C HuC6280 CPU, VDC/VCE and PSG for HuCards | `4eee99c451f8` | GPL-2.0-or-later, elected GPLv3 for the app; `COPYING` and `UPSTREAM.md` |
| `totalsms/` | [TotalSMS](https://github.com/ITotalJustice/TotalSMS), Master System / Game Gear CPU/VDP and cartridge mappers; scheduler dependency | `05bbb8bbd931` | MIT; pins and port changes in `totalsms/UPSTREAM.md`; original integer PSG in `packages/mastersystem/psg.c` |
| `clownmdemu/` | [ClownMDEmu](https://github.com/Clownacy/clownmdemu), Genesis CPU/VDP/FM/PSG, Clown68000 and ClownZ80 | `88ef45a65855` | AGPL-3.0-or-later; clowncommon ISC; dependency pins and port changes in `clownmdemu/UPSTREAM.md` |
| `gnuboy/` | [gnuboy](https://github.com/rofl0r/gnuboy), DMG/CGB CPU, graphics and integer APU | `4c2f6c441eb3` | GPL-2.0-or-later, `gnuboy/COPYING`; election in `NOTICE`, local changes in `UPSTREAM.md` |
| `infones/` | [InfoNES](https://github.com/jay-kumogata/InfoNES), CPU/PPU/integer APU and mappers 0–4 | `fe3295c0a86b` | Apache-2.0, `infones/LICENSE`; port changes in `infones/UPSTREAM.md` |

InfoNES has the C portability, bounds and CPU correctness changes documented
in `infones/UPSTREAM.md`.
