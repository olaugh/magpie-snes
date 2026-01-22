# Prime Sieve ROM Makefile
# Cross-compiles C code to a SNES ROM using cc65

# cc65 toolchain
CC = cc65
CA = ca65
LD = ld65

# Flags
CFLAGS = -t none -Oi -Os --cpu 65816
ASFLAGS = --cpu 65816
# cc65 runtime library (freestanding)
CC65_LIB = /opt/homebrew/Cellar/cc65/2.19/share/cc65/lib/none.lib
LDFLAGS = -C snes.cfg

# Output
TARGET = prime_sieve
ROM = $(TARGET).sfc
HEADER = prime_sieve_rom.h
SYMBOLS_HEADER = prime_sieve_symbols.h

# Source files
CSRC = main.c
ASRC = crt0.s

# Object files
OBJS = crt0.o main.o

.PHONY: all clean header install

all: $(ROM) $(HEADER)

# Compile C to assembly, then assemble
main.o: main.c
	$(CC) $(CFLAGS) -o main.s $<
	$(CA) $(ASFLAGS) -o $@ main.s

# Assemble startup code
crt0.o: crt0.s snes.inc
	$(CA) $(ASFLAGS) -o $@ $<

# Link to ROM
$(ROM): $(OBJS) snes.cfg
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(CC65_LIB)
	@echo "ROM size: $$(stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes"

# Generate C++ header with embedded ROM data
$(HEADER): $(ROM)
	@echo "// Auto-generated from $(ROM)" > $@
	@echo "// Prime Sieve test ROM for SNES" >> $@
	@echo "" >> $@
	@echo "#ifndef PRIME_SIEVE_ROM_H" >> $@
	@echo "#define PRIME_SIEVE_ROM_H" >> $@
	@echo "" >> $@
	@echo "#include <cstdint>" >> $@
	@echo "#include <cstddef>" >> $@
	@echo "" >> $@
	@echo "namespace SX {" >> $@
	@echo "namespace TestRoms {" >> $@
	@echo "" >> $@
	@echo "// Prime Sieve ROM ($$(stat -f%z $(ROM) 2>/dev/null || stat -c%s $(ROM)) bytes)" >> $@
	@echo "// Computes first 100 primes using Sieve of Eratosthenes" >> $@
	@echo "// Results written to WRAM:" >> $@
	@echo "//   \$$7E0400-\$$7E04C7: Prime values (100 x 16-bit words)" >> $@
	@echo "//   \$$7E0600: Prime count" >> $@
	@echo "//   \$$7E0602: Done flag (\$$DEAD when complete)" >> $@
	@echo "" >> $@
	@echo "constexpr size_t PRIME_SIEVE_ROM_SIZE = $$(stat -f%z $(ROM) 2>/dev/null || stat -c%s $(ROM));" >> $@
	@echo "" >> $@
	@echo "constexpr uint8_t PRIME_SIEVE_ROM[] = {" >> $@
	@xxd -i < $(ROM) | sed 's/^/    /' >> $@
	@echo "};" >> $@
	@echo "" >> $@
	@echo "// First 100 prime numbers for verification" >> $@
	@echo "constexpr uint16_t EXPECTED_PRIMES[] = {" >> $@
	@echo "    2, 3, 5, 7, 11, 13, 17, 19, 23, 29," >> $@
	@echo "    31, 37, 41, 43, 47, 53, 59, 61, 67, 71," >> $@
	@echo "    73, 79, 83, 89, 97, 101, 103, 107, 109, 113," >> $@
	@echo "    127, 131, 137, 139, 149, 151, 157, 163, 167, 173," >> $@
	@echo "    179, 181, 191, 193, 197, 199, 211, 223, 227, 229," >> $@
	@echo "    233, 239, 241, 251, 257, 263, 269, 271, 277, 281," >> $@
	@echo "    283, 293, 307, 311, 313, 317, 331, 337, 347, 349," >> $@
	@echo "    353, 359, 367, 373, 379, 383, 389, 397, 401, 409," >> $@
	@echo "    419, 421, 431, 433, 439, 443, 449, 457, 461, 463," >> $@
	@echo "    467, 479, 487, 491, 499, 503, 509, 521, 523, 541" >> $@
	@echo "};" >> $@
	@echo "" >> $@
	@echo "constexpr size_t NUM_PRIMES = 100;" >> $@
	@echo "" >> $@
	@echo "// Memory addresses for test verification (SNES WRAM)" >> $@
	@echo "// Low WRAM (\$$0000-\$$1FFF) is mirrored at \$$7E0000-\$$7E1FFF" >> $@
	@echo "constexpr uint32_t PRIME_RESULTS_ADDR = 0x7E0400;" >> $@
	@echo "constexpr uint32_t PRIME_COUNT_ADDR = 0x7E0600;" >> $@
	@echo "constexpr uint32_t DONE_FLAG_ADDR = 0x7E0602;" >> $@
	@echo "constexpr uint16_t DONE_FLAG_VALUE = 0xDEAD;" >> $@
	@echo "" >> $@
	@echo "} // namespace TestRoms" >> $@
	@echo "} // namespace SX" >> $@
	@echo "" >> $@
	@echo "#endif // PRIME_SIEVE_ROM_H" >> $@
	@echo "Generated $(HEADER)"

clean:
	rm -f $(OBJS) main.s $(ROM) $(HEADER) $(SYMBOLS_HEADER)

# Install header to tests directory
install: $(HEADER)
	cp $(HEADER) ../../tests/
	@echo "Installed $(HEADER) to tests/"
