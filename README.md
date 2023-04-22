# CHIP-8
A Chip-8 Emulator Written In C

---
## About
this is my first step into low-level stuff, i look forward on building more emulators & once i have some money i want to build a small computer with a 6502 or a z80.

chip-8 is kinda like a "specification" which decribes how a chip-8 machine should work, i am not aware of any "true" chip-8 cpu but it's quite easy to build a emulator of.

---
## Accuracy
i tried to make the emulator as accurate as possible and for that i used [chip-8 test suite](https://github.com/Timendus/chip8-test-suite), which provides many roms to test various things like opcodes, flags etc.

<p float="left" align="center">
  <img src="https://user-images.githubusercontent.com/75035219/233769394-e036580c-a842-4417-b6ba-6bbe3cd92f40.png" width="400" />
  <img src="https://user-images.githubusercontent.com/75035219/233769411-1da884ee-434c-488b-997a-6535927f5ccf.png" width="400" />
  <img src="https://user-images.githubusercontent.com/75035219/233769419-b9617671-7d4a-4b0f-959c-689eadbb0d27.png" width="400" />
  <img src="https://user-images.githubusercontent.com/75035219/233769511-e8bb6aa2-314e-4200-8bdd-edc90ebd62e3.png" width="400" />
  <img src="https://user-images.githubusercontent.com/75035219/233769540-ba5c4923-eb6e-4513-b019-d4fa93ef0c70.png" width="400" />
</p>

---
## Keymap
i'm using this keymap:

```
     CHIP-8              My Keyboard
 _______________       _______________
| 1 | 2 | 3 | C |     | 1 | 2 | 3 | 4 |
|---|---|---|---|     |---|---|---|---|
| 4 | 5 | 6 | D |     | Q | W | E | R |
|---|---|---|---| --> |---|---|---|---|
| 7 | 8 | 9 | E |     | A | S | D | F |
|---|---|---|---|     |---|---|---|---|
| A | 0 | B | F |     | Z | X | C | V |
\---------------/     \---------------/
```

---
## Building

make sure you have GNU Make, GCC/Clang & SDL2 Installed, then clone the repo

```
make all -j4
```

you will have a executable named `chip-8`, run the executable and pass a path to a rom...

---
## License
this project is Licensed under [BSD 3-Clause](./LICENSE) License.

---
# Thanks
