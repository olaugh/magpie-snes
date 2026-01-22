/**
 * sxtest - Prime Sieve ROM test
 *
 * This test verifies that the SNES emulator correctly executes
 * a C program compiled with cc65 that computes prime numbers.
 */

#include <sxtest.h>
#include "prime_sieve_rom.h"

namespace {

using namespace SX::TestRoms;

class PrimeSieveTest : public SX::Test {
protected:
    void SetUp() override {
        bool loaded = LoadRom(PRIME_SIEVE_ROM, PRIME_SIEVE_ROM_SIZE);
        ASSERT_TRUE(loaded) << "Failed to load prime sieve ROM";
    }
};

TEST_F(PrimeSieveTest, RomLoads) {
    EXPECT_TRUE(emu.IsRomLoaded());
}

TEST_F(PrimeSieveTest, WRAMIsAccessible) {
    // Run a few frames to let the ROM initialize
    RunFrames(10);

    // WRAM should be accessible
    uint8_t* wram = emu.GetWRAM();
    EXPECT_NE(wram, nullptr) << "WRAM should be accessible";
}

TEST_F(PrimeSieveTest, ComputesPrimes) {
    // Run until done flag is set or timeout
    int frame = emu.RunUntilMemoryEquals(DONE_FLAG_ADDR, 0xAD, 600);

    // Check that we didn't timeout
    ASSERT_GE(frame, 0) << "Timed out waiting for done flag";

    // Verify done flag is complete (both bytes)
    uint16_t done_flag = ReadWord(DONE_FLAG_ADDR);
    EXPECT_EQ(done_flag, DONE_FLAG_VALUE) << "Done flag should be 0xDEAD";

    // Verify prime count
    uint16_t prime_count = ReadWord(PRIME_COUNT_ADDR);
    EXPECT_EQ(prime_count, NUM_PRIMES) << "Should compute 100 primes";

    // Verify each prime number
    for (size_t i = 0; i < NUM_PRIMES; i++) {
        uint16_t computed = ReadWord(PRIME_RESULTS_ADDR + i * 2);
        EXPECT_EQ(computed, EXPECTED_PRIMES[i])
            << "Prime[" << i << "] mismatch: expected "
            << EXPECTED_PRIMES[i] << ", got " << computed;
    }
}

TEST_F(PrimeSieveTest, FirstPrimeIs2) {
    // Run until done
    emu.RunUntilMemoryEquals(DONE_FLAG_ADDR, 0xAD, 600);

    uint16_t first_prime = ReadWord(PRIME_RESULTS_ADDR);
    EXPECT_EQ(first_prime, 2) << "First prime should be 2";
}

TEST_F(PrimeSieveTest, LastPrimeIs541) {
    // Run until done
    emu.RunUntilMemoryEquals(DONE_FLAG_ADDR, 0xAD, 600);

    uint16_t last_prime = ReadWord(PRIME_RESULTS_ADDR + 99 * 2);
    EXPECT_EQ(last_prime, 541) << "100th prime should be 541";
}

TEST_F(PrimeSieveTest, Performance) {
    // Time how long it takes to compute primes
    int frames = emu.RunUntilMemoryEquals(DONE_FLAG_ADDR, 0xAD, 600);

    ASSERT_GE(frames, 0) << "Timed out";

    // At 60fps, should complete well within 300 frames (5 seconds)
    EXPECT_LT(frames, 300) << "Should complete within 5 seconds of emulated time";
}

} // namespace
