# sxtest Feature Parity Plan

This document tracks the work needed to bring sxtest (SNES) to feature parity with gxtest (Genesis).

## Current Status

### What sxtest Has
- [x] Bazel build system with GoogleTest
- [x] Emulator wrapper class (`SX::Emulator`)
- [x] ROM loading (file and memory buffer)
- [x] Frame execution (`RunFrames`, `RunUntilMemoryEquals`, `RunUntil`)
- [x] Memory access (`ReadByte`, `ReadWord`, `WriteByte`, `WriteWord`)
- [x] Direct RAM access (`GetWRAM`, `GetVRAM`)
- [x] Input control (12 SNES buttons)
- [x] State save/load
- [x] Test fixture base class (`SX::Test`)
- [x] Embedded test ROMs (minimal, prime_sieve)
- [x] cc65 toolchain for ROM building

### What gxtest Has That sxtest Is Missing

## Phase 1: API Parity (Core Emulator Features)

### 1.1 Additional Memory Access Methods
- [ ] `ReadLong(uint32_t address)` - Read 32-bit value (little-endian for SNES)
- [ ] `WriteLong(uint32_t address, uint32_t value)` - Write 32-bit value

**Effort:** Low
**Files:** `include/sxtest.h`, `src/sxtest.cpp`

### 1.2 CPU Register Access
- [ ] `GetDataRegister(int reg)` - Not applicable for 65816 (no D registers)
- [ ] `GetAddressRegister(int reg)` - Not applicable for 65816
- [ ] `GetAccumulator()` - 16-bit A register
- [ ] `GetX()` - 16-bit X register
- [ ] `GetY()` - 16-bit Y register
- [ ] `GetPC()` - Program counter
- [ ] `GetSP()` - Stack pointer
- [ ] `GetDP()` - Direct page register
- [ ] `GetDB()` - Data bank register
- [ ] `GetPB()` - Program bank register
- [ ] `GetP()` - Processor status register

**Note:** The 65816 has different registers than the 68000. Need to map to snes9x internals.

**Effort:** Medium - requires understanding snes9x register access
**Files:** `include/sxtest.h`, `src/sxtest.cpp`

### 1.3 Hard Reset Support
- [ ] `HardReset()` - Full power cycle (vs soft `Reset()`)

**Effort:** Low
**Files:** `include/sxtest.h`, `src/sxtest.cpp`

### 1.4 Test Fixture Convenience Methods
- [ ] Add register access wrappers to `SX::Test` class

**Effort:** Low
**Files:** `include/sxtest.h`

---

## Phase 2: Profiler Support

gxtest includes a CPU cycle profiler that hooks into the emulator's instruction execution. This is powerful for performance analysis.

### 2.1 Profiler Class
- [ ] Create `include/profiler.h` with `SX::Profiler` class
- [ ] Create `src/profiler.cpp` implementation

**Features needed:**
- [ ] Symbol table management (`AddFunction`, `ClearSymbols`)
- [ ] ELF symbol loading (`LoadSymbolsFromELF`) - may need adaptation for 65816 ELF
- [ ] Symbol file loading (`LoadSymbolsFromFile`)
- [ ] Profiling control (`Start`, `Stop`, `Reset`, `IsRunning`)
- [ ] Statistics retrieval (`GetStats`, `GetAllStats`, `GetTotalCycles`)
- [ ] Profile modes (`Simple` for exclusive cycles, `CallStack` for inclusive)
- [ ] Sample rate control for performance
- [ ] Address histogram for line-level profiling
- [ ] Report generation (`PrintReport`)
- [ ] JSON export for visualization tools

**Effort:** High - requires CPU hook support in snes9x libretro
**Challenge:** snes9x libretro may not expose CPU hooks like Genesis Plus GX does

### 2.2 CPU Hook Integration
- [ ] Research snes9x libretro API for CPU hooks
- [ ] If not available, consider:
  - Building snes9x from source with custom hooks
  - Using a different SNES emulator core (bsnes-mercury has better debugging)
  - Implementing sampling-based profiling instead

**Effort:** High
**Risk:** May require emulator core modifications

### 2.3 Build System Integration
- [ ] Add profiler to `BUILD.bazel`
- [ ] Add profiler tests

**Effort:** Low

---

## Phase 3: Symbol Tracking

gxtest has tools to extract symbols from ELF files and generate C++ headers with memory addresses.

### 3.1 Symbol Extraction Tool
- [ ] Create `tools/cc65sym.py` - Extract symbols from cc65 map files
  - cc65 uses `.map` files with different format than `nm` output
  - Need to parse cc65's linker map format

**Effort:** Medium
**Files:** `tools/cc65sym.py`

### 3.2 Alternative: nm-compatible Output
- [ ] Research if cc65's `ld65` can produce nm-compatible output
- [ ] Or use ca65's `.dbg` debug info files

**Effort:** Medium

### 3.3 Symbol Header Generation
- [ ] Adapt symbol header generation for SNES memory map
  - WRAM at $7E0000-$7FFFFF vs Genesis $FF0000-$FFFFFF
  - Direct page at $00-$FF
  - Filter for relevant memory regions

**Effort:** Low

### 3.4 Symbol Example ROM
- [ ] Create `roms/symbol_example/` with game-like test ROM
- [ ] Generate symbol headers
- [ ] Create symbol-based tests

**Effort:** Medium
**Files:** `roms/symbol_example/`, `tests/symbol_example_test.cpp`

---

## Phase 4: Documentation & Polish

### 4.1 README
- [ ] Create comprehensive README.md
- [ ] Document all API methods
- [ ] Include usage examples
- [ ] Document thread safety warnings

**Effort:** Medium
**Files:** `README.md`

### 4.2 API Documentation
- [ ] Add Doxygen-style comments to all public methods
- [ ] Document SNES memory map in header

**Effort:** Low

### 4.3 Additional Tests
- [ ] Add more edge case tests
- [ ] Add performance benchmark tests
- [ ] Add state save/load stress tests

**Effort:** Medium

---

## Priority Order

1. **Phase 1.1** - ReadLong/WriteLong (immediate utility)
2. **Phase 1.3** - HardReset (simple, matches API)
3. **Phase 3.1** - Symbol extraction tool (enables symbol-based testing)
4. **Phase 1.2** - CPU register access (useful for debugging)
5. **Phase 3.4** - Symbol example ROM
6. **Phase 4.1** - README documentation
7. **Phase 2** - Profiler (most complex, may need emulator changes)

---

## Technical Notes

### snes9x vs Genesis Plus GX

| Feature | Genesis Plus GX | snes9x libretro |
|---------|-----------------|-----------------|
| Build method | Source in tree | Prebuilt static lib |
| CPU hooks | Yes (HOOK_CPU define) | Unknown |
| Memory access | Direct globals | libretro API |
| Register access | m68k_get_reg() | Need to research |

### cc65 vs m68k-elf-gcc

| Feature | m68k-elf-gcc | cc65 |
|---------|--------------|------|
| Output format | ELF | Custom binary |
| Symbol extraction | `nm` | `.map` file |
| Debug info | DWARF | `.dbg` file |
| Optimization | Standard GCC | Limited |

### Memory Map Differences

| Region | Genesis | SNES |
|--------|---------|------|
| Work RAM | $FF0000-$FFFFFF (64KB) | $7E0000-$7FFFFF (128KB) |
| Sound RAM | $A00000-$A01FFF (8KB Z80) | $000000-$00FFFF (APU, separate) |
| VRAM | VDP registers | $000000-$FFFF (64KB, separate) |
| ROM | $000000-$3FFFFF | Bank-switched |

---

## Estimated Timeline

| Phase | Effort | Dependencies |
|-------|--------|--------------|
| 1.1 ReadLong | 1 hour | None |
| 1.3 HardReset | 30 min | None |
| 1.2 CPU Registers | 4 hours | Research snes9x internals |
| 1.4 Test Wrappers | 30 min | 1.2 |
| 3.1 Symbol Tool | 3 hours | None |
| 3.4 Symbol Example | 4 hours | 3.1 |
| 4.1 README | 2 hours | None |
| 2.x Profiler | 8+ hours | CPU hook research |

**Total: ~24 hours** (excluding profiler research)
