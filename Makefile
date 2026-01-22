# SNES Scrabble - cc65/ca65 Build System
# Magpie Scrabble AI Port

# Toolchain
CC = cc65
CA = ca65
LD = ld65

# Directories
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
OUT_DIR = out
DATA_DIR = data

# Source files (exclude test ROMs from main build)
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES = $(filter-out $(SRC_DIR)/test_minimal.s $(SRC_DIR)/test_simple.s, $(wildcard $(SRC_DIR)/*.s))

# Object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler flags
CFLAGS = -t none -Oi -Os --cpu 65816
CFLAGS += -I $(INC_DIR)
CFLAGS += --add-source

# Assembler flags
ASFLAGS = --cpu 65816 -I $(SRC_DIR)

# Linker flags
# Use none.lib for freestanding runtime functions
CC65_LIB = /opt/homebrew/Cellar/cc65/2.19/share/cc65/lib/none.lib
LDFLAGS = -C snes.cfg

# ROM output
ROM = $(OUT_DIR)/scrabble.sfc

# Default target
all: dirs $(ROM)
	@echo "Build complete: $(ROM)"
	@ls -la $(ROM)

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR) $(OUT_DIR)

# Compile C to assembly, then assemble
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$*.s $<
	$(CA) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.s

# Assemble
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s | dirs
	$(CA) $(ASFLAGS) -o $@ $<

# Link
$(ROM): $(OBJECTS) snes.cfg | dirs
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(CC65_LIB)
	@echo "ROM size: $$(stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)

# Show object files
list:
	@echo "C sources: $(C_SOURCES)"
	@echo "ASM sources: $(ASM_SOURCES)"
	@echo "Objects: $(OBJECTS)"

# Generate KWG data (converts binary to C array)
$(BUILD_DIR)/kwg_data.c: $(DATA_DIR)/lexica/NWL23.kwg | dirs
	python3 tools/kwg2c.py $< $@

# Generate KLV data
$(BUILD_DIR)/klv_data.c: $(DATA_DIR)/NWL23.klv16 | dirs
	python3 tools/klv2c.py $< $@

# Test with bsnes (if installed)
run: $(ROM)
	@if command -v bsnes >/dev/null 2>&1; then \
		bsnes $(ROM); \
	elif command -v bsnes-plus >/dev/null 2>&1; then \
		bsnes-plus $(ROM); \
	else \
		echo "No SNES emulator found. Install bsnes or bsnes-plus."; \
		echo "On macOS: brew install --cask bsnes"; \
	fi

# Build test_minimal ROM separately
test_minimal: dirs $(OUT_DIR)/test_minimal.sfc
	@echo "Test ROM built: $(OUT_DIR)/test_minimal.sfc"

$(OUT_DIR)/test_minimal.sfc: $(BUILD_DIR)/test_minimal.o snes.cfg | dirs
	$(LD) $(LDFLAGS) -o $@ $(BUILD_DIR)/test_minimal.o

# Build test_simple ROM (ultra-minimal)
test_simple: dirs $(OUT_DIR)/test_simple.sfc
	@echo "Simple test ROM built: $(OUT_DIR)/test_simple.sfc"

$(OUT_DIR)/test_simple.sfc: $(BUILD_DIR)/test_simple.o snes.cfg | dirs
	$(LD) $(LDFLAGS) -o $@ $(BUILD_DIR)/test_simple.o

.PHONY: all clean dirs list run test_minimal test_simple
