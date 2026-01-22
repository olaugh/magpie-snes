# magpie-snes

A port of the Magpie Scrabble AI engine to the Super Nintendo Entertainment System (SNES).

## Overview

Magpie is a high-performance Scrabble AI that uses sophisticated move generation and evaluation algorithms. This project ports the core engine to run natively on SNES hardware using the 65816 processor.

## Project Structure

```
magpie-snes/
├── src/                    # SNES source code
│   ├── main.c              # Main entry point
│   ├── graphics.c          # PPU/graphics routines
│   ├── vectors.s           # CPU vectors and startup code
│   └── test_minimal.s      # Minimal test ROM
├── inc/                    # Header files
│   ├── snes.h              # SNES hardware definitions
│   └── ppu.h               # PPU register helpers
├── data/                   # Game data
│   ├── NWL23.klv16         # Leave values (16-bit format)
│   └── lexica/             # Word list data (KWG format)
├── tools/                  # Build tools
│   ├── kwg2c.py            # KWG to C array converter
│   └── klv2c.py            # KLV to C array converter
├── sxtest/                 # Headless test harness
│   ├── include/            # Test framework headers
│   ├── src/                # Test framework implementation
│   ├── tests/              # Test cases
│   └── roms/               # Test ROM sources
├── build/                  # Build artifacts (generated)
├── out/                    # ROM output (generated)
├── snes.cfg                # cc65 linker configuration
└── Makefile                # Build system
```

## Building

### Prerequisites

- [cc65](https://cc65.github.io/) - 6502/65816 C compiler suite
- Python 3 (for data conversion tools)

On macOS:
```bash
brew install cc65
```

### Build the ROM

```bash
make
```

This produces `out/scrabble.sfc`.

### Build test ROM

```bash
make test_minimal
```

This produces `out/test_minimal.sfc` - a minimal ROM that displays a blue screen.

### Run in emulator

```bash
make run
```

Requires bsnes or another SNES emulator.

## Testing

The project includes **sxtest**, a headless testing framework for SNES ROMs using the snes9x emulator core. This enables automated testing without a GUI.

### Running tests

```bash
cd sxtest
bazel test :sxtest_minimal :sxtest_prime_sieve
```

### Test framework features

- GoogleTest integration
- Memory read/write verification
- Frame-by-frame execution
- Conditional execution (run until memory equals value)
- State save/load
- Input simulation

See [sxtest/PARITY_PLAN.md](sxtest/PARITY_PLAN.md) for the test framework roadmap.

## Development Status

**Phase 1: Bootstrap** (current)
- [x] cc65 toolchain setup
- [x] Basic PPU initialization
- [x] Text rendering
- [x] Controller input
- [x] Test harness (sxtest)
- [ ] Memory management
- [ ] Board display

**Phase 2: Core Engine**
- [ ] KWG lexicon loading
- [ ] Board representation
- [ ] Move generation (GADDAG)
- [ ] Move validation

**Phase 3: AI**
- [ ] Leave value evaluation
- [ ] Move scoring
- [ ] Best move selection
- [ ] Endgame solver

**Phase 4: Game**
- [ ] Full game loop
- [ ] Tile bag management
- [ ] Score tracking
- [ ] Game save/load

## Technical Notes

### Memory Map

The SNES has a complex memory map. This project uses LoROM mapping:

| Address Range | Contents |
|--------------|----------|
| $00-$3F:$8000-$FFFF | ROM (banks 0-63) |
| $7E:$0000-$1FFF | WRAM (first 8KB, mirrored at $00-$3F:$0000-$1FFF) |
| $7E:$0000-$FFFF | WRAM (full 128KB) |
| $7F:$0000-$FFFF | WRAM (continued) |

### Toolchain

- **cc65** - C compiler targeting 65816
- **ca65** - Assembler with macro support
- **ld65** - Linker with flexible memory configuration

### Limitations

The 65816 processor and SNES hardware present unique challenges:
- 64KB addressable at a time (bank switching required)
- No hardware multiply/divide (use lookup tables)
- Limited RAM (128KB WRAM)
- 8-bit accumulator by default (16-bit mode available)

## Related Projects

- [Magpie](https://github.com/olaugh/magpie) - Original Scrabble AI engine
- [gxtest](https://github.com/olaugh/gxtest) - Genesis/Mega Drive test harness (sxtest is modeled after this)

## License

TBD
