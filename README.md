# CHIP-8 Emulator

A straightforward and accurate CHIP-8 emulator written in C++ using [Raylib](https://www.raylib.com/). 

This project was built to accurately interpret and execute classic CHIP-8 ROMs. It includes full support for the standard instruction set, timers, audio, and a custom visual effect to simulate the display of older hardware.

## Features
* **Accurate Emulation:** Runs classic CHIP-8 games (Pong, Space Invaders, Tetris, etc.) at a stable 60Hz timer rate.
* **Phosphor Decay Effect:** Implements a customizable CRT-style visual ghosting effect. This eliminates the harsh sprite flickering present in many older CHIP-8 games that rely on drawing/erasing to simulate grayscale.
* **Audio Support:** Accurate sound timer emulation using Raylib's audio engine.
* **CLI ROM Loading:** Easily load any `.ch8` file directly from the command line.

## Requirements
To compile and run this project, you need:
* GCC
* [Raylib](https://github.com/raysan5/raylib) installed and linked on your system.

## Building from Source

You can compile the project from the terminal. Make sure you link the Raylib library. 

```bash
g++ main.cpp -o chip8 -lraylib -lm -lpthread -ldl -lrt -lX11
```

## Usage

Run the emulator from the command line by passing the path to a CHIP-8 ROM as an argument:

```bash
./chip8 path/to/rom.ch8
```

## Controls

The original CHIP-8 used a 16-key hexadecimal keypad (0-F). This emulator maps those keys to the left side of a modern QWERTY keyboard:

| Original CHIP-8 Keypad | Mapped PC Keyboard |
| :---: | :---: |
| `1` `2` `3` `C` | `1` `2` `3` `4` |
| `4` `5` `6` `D` | `Q` `W` `E` `R` |
| `7` `8` `9` `E` | `A` `S` `D` `F` |
| `A` `0` `B` `F` | `Z` `X` `C` `V` |

## Configuration
If you want to tweak the emulator's visuals, you can edit the following variables inside `main.cpp` before compiling:
* `scale` (default `30`): Changes the size of the application window.
* `decay_rate` (default `20.0f`): Controls the speed of the phosphor ghosting effect. Lower numbers result in a longer "trail" behind moving objects, while higher numbers fade faster.