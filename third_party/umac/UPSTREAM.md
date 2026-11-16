uMac https://github.com/evansm7/umac
Pin: b62d3c6e725ca2ac2224e710b3cee765dbc35e45
Musashi pin: b3144f12c1296d8fbb600eea13f5db71ffb1b8d8

Port changes: bytewise ROM identification; GCC builtin nonlocal jump shim; fatal errors return to frontend; eject ends session; initialization resets peripheral state. See source licenses: MIT except Basilisk II disk driver and Mini vMac keymaps (GPLv2). The compiled disk driver has a GPLv2-or-later grant, elected GPLv3 for the app. The GPLv2 Mini vMac keymaps and SDL frontend are retained as upstream reference files and are not compiled into the app.

Additional port changes: disable unused FPU/MMU includes and disassembly hook; remove unused disassembler helpers; fix signed shifts in opcode templates and RAM long reads; bound disk transfers and report actual byte counts; reset VIA/SCC, overlay and mouse state on session start; FIFO for keyboard transitions. SDL frontend retained as reference only. Generated opcode sources live under out/.

Absolute touchscreen API writes classic Mac MTemp (0x828), RawMouse (0x82c) and CrsrNew/CrsrCouple (0x8ce/0x8cf), bypassing relative acceleration; waits for SCC mouse enable before changing RAM. Interface reference: https://github.com/minivmac/minivmac/blob/master/src/MOUSEMDV.c (interface facts only; implementation written for this port).
