The main Makefile includes another makefile named `ComputerSpecific.mak`, which should hold paths to the 65816 assembler, 65816 linker, and SNES emulator.

Specifically, the main Makefile expects the following variables:

* `WLA_65c816` - should contain path to `wla-65816.exe`
* `WLA_LINKER` - should contain path to `wlalink.exe`
* `EMULATOR` - should contain path to SNES emulator executable.

Range of valid versions for `wla-65816` and `wlalink` is unknown; it's known that version *10.4 Win32* works.

Any emulator that can accept a `.smc` file as its first command line argument (like so: `emulator.exe test.smc`) should work; has been tested with BizHawk-2.8-win-x64.

Example `ComputerSpecific.mak`:
```
WLA_65c816 = wla-65816
WLA_LINKER = wlalink
EMULATOR = E:\Games\BizHawk\BizHawk-2.8-win-x64\EmuHawk.exe
```