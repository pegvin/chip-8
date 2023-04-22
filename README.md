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
  <img src="https://user-images.githubusercontent.com/75035219/233769853-2b4fee82-a1ff-422f-96bf-c78a2d0d8e80.png" width="400" />&nbsp;
  <img src="https://user-images.githubusercontent.com/75035219/233769888-d29a0f03-8f79-4fd4-8d25-e34153e1d1b5.png" width="400" />&nbsp;
  <img src="https://user-images.githubusercontent.com/75035219/233769913-90a6c59d-c321-4f37-819a-f5d57dd2e7d0.png" width="400" />&nbsp;
  <img src="https://user-images.githubusercontent.com/75035219/233769945-19d48233-a0aa-4b9d-8ec3-7505128d2402.png" width="400" />&nbsp;
  <img src="https://user-images.githubusercontent.com/75035219/233769966-8f748245-fd1f-4c3e-a071-dae4b1b0b5ab.png" width="400" />
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
