# Sudoku in C with PDCurses

A terminal Sudoku game for Windows, written in C and using the PDCurses library. Yes, this was made entirely with AI.

## Features

- Playable 9x9 board in the terminal
- Arrow-key navigation
- Highlighted fixed numbers
- Visual conflicts shown in red
- Reset the current puzzle
- Randomly select a new puzzle

## Controls

- `Arrow keys`: move the selection
- `1` to `9`: fill the cell
- `0`, `Backspace`, or `Delete`: clear the cell
- `N`: new game
- `R`: reset the current puzzle
- `Q` or `Esc`: quit

## Building on Windows

With MinGW and PDCurses installed:

```bat
gcc main.c sudoku.c -o sudoku.exe -lpdcurses
```

If your distribution uses a different name for the library, replace the final parameter, for example:

```bat
gcc main.c sudoku.c -o sudoku.exe -lpdcurseswin
```
