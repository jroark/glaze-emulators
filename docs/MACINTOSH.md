# Macintosh

`macintosh.gzp` runs uMac with 512 KB RAM and a 512×342 monochrome screen on
Glaze for the Casio BE-300. Build with `./build.sh`; install `out/pkg/macintosh.gzp`
through Files like the other emulator packages.

Supply an unmodified 128 KB Mac Plus V3 ROM (header checksum `4d1f8172`) and a
raw `.img` or `.dsk` boot disk on `/Card`. Extract archives first. System 3.2
with Finder 5.3 is tested in the native core runner. No Apple software is bundled.
Select the ROM and disk in the app, then Start Macintosh. Root opens the
filesystem root; Prev/Next paginate the file picker. A disk must be writable.

**Guest writes immediately modify the selected image. Use a copy for experiments.**
Disk Copy containers and compressed images are unsupported. One disk is attached;
guest eject returns to setup. A disk error stops the session with an error message.

The toolbar selects Fit or Zoom (1:1), Mouse or Pan, the keyboard, Pause/Play,
and Reset. Mouse mode positions the Mac cursor at the touch point: touch to click, drag
to move with the stylus, and lift to release. Fit scaling and zoom viewport offsets
are applied to Glaze's app-space touch coordinates. SpringBoard handles rotation. Letterboxing does
not click the desktop; dragging outside it clamps to its edge. In Zoom, Pan drags the viewport without
sending mouse clicks. Fit preserves the full desktop aspect ratio.

Landscape uses Glaze's native orientation support from
[glaze-os PR #133](https://github.com/jroark/glaze-os/pull/133). Double-press
Home, swipe the multitasking tray right, and tap its rotation control. Return
to Macintosh to apply the setting. The native landscape surface is 320×224;
SpringBoard rotates the app, status bar and touch coordinates. The app adapts
its viewport, controls, file picker and keyboard to the new size on resume.
Use the same tray control to switch back to portrait. There is no app-specific
rotation setting.

Build and run against the SDK and Glaze runtime containing PR #133. With that
checkout in another location, use `GLAZE_DIR=/path/to/glaze-os ./build.sh` (the
checkout must first export its SDK with `./build.sh sdk`).

Keys opens the Glaze keyboard and sticky Mac Shift, Command, Option and Control
buttons, plus Escape. Physical ASCII input, Return, Tab, Backspace, Escape and
arrows are mapped when supplied by Glaze. Home remains the Glaze launcher key.
Pause, Home and leaving the session release guest input. Reset cold-boots RAM
while retaining disk changes. There are no saved machine states.

Upstream uMac does not emulate sound, SCSI, networking, VIA timers or multiple
active disks. This port uses its interpreter without cycle accounting; speed
is hardware-dependent and is not guaranteed to match an original Macintosh.

## Tests

`make test` includes sanitizer checks of the disk and display adapters and key
mapping. An optional sanitizer core boot test uses private media without changing
it (guest writes go into an in-memory copy, up to an 800 KB disk):

```
make -C tests macintosh-boot MAC_ROM=/absolute/path/plus-v3.rom MAC_DISK=/absolute/path/system.img
```

It runs two fresh sessions of 60 simulated seconds each and writes `out/host/macintosh-boot.pgm` for visual
inspection. A Finder desktop in that image establishes core boot success; the
runner alone cannot establish touchscreen operation or hardware performance.

The packaged app has been launched in the BE-300 emulator using a separate
Glaze image with scripted setup. Native ROM tests verify absolute guest cursor
coordinates and repeated boot; adapter tests cover rendering and touch transforms
at both native surface sizes. The native UI suite covers keyboard scaling and
ACTIVATE→RESIZE after changing orientation while suspended. Guest document saving through the packaged app and
physical BE-300 performance still require end-to-end verification.

`tests/ui/t_macintosh_landscape.txt` is the native landscape smoke fixture.
With the card contents described in that script, build Glaze with
`INPUT_SCRIPT=/absolute/path/to/t_macintosh_landscape.txt`, boot for 45 seconds,
and check the log for `macintosh: resized 320x224`. The saved LCD screenshot
includes the natively rotated status bar and Macintosh controls.
