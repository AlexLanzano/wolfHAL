# wolfHAL Examples

## Building

From an example directory:

```
cd blinky
make BOARD=<board>
```

The output binary is placed in `build/<board>/`.

## Example Structure

Each example is self-contained with its own board support:

```
<example>/
  main.c
  Makefile
  boards/
    <board>/
      board.c     # Device instances and Board_Init
      board.h     # Externs and board constants
      Makefile.inc
      linker.ld
      ...         # Additional board-specific source files
```

## Adding a New Board

1. Create `boards/<board_name>/` with `board.c`, `board.h`, `Makefile.inc`, `linker.ld`, and any additional board-specific source files.
2. Define the device instances and `Board_Init` needed by the example in `board.c`.
3. Build with `make BOARD=<board_name>`.
