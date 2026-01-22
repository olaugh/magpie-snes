/**
 * sxtest - Example test file
 *
 * This file demonstrates how to use the sxtest framework to write
 * unit tests for SNES ROMs.
 */

#include <sxtest.h>

namespace {

// ---------------------------------------------------------------------------
// Basic Emulator Tests (no ROM required)
// ---------------------------------------------------------------------------

TEST(SXTestBasic, EmulatorInitializes) {
    SX::Emulator emu;
    EXPECT_FALSE(emu.IsRomLoaded());
    EXPECT_EQ(emu.GetFrameCount(), 0u);
}

TEST(SXTestBasic, LoadNonexistentRomFails) {
    SX::Emulator emu;
    EXPECT_FALSE(emu.LoadRom("/nonexistent/path/to/rom.sfc"));
    EXPECT_FALSE(emu.IsRomLoaded());
}

// ---------------------------------------------------------------------------
// Example ROM Test Fixture
// ---------------------------------------------------------------------------

/**
 * Example test fixture that demonstrates the recommended pattern
 * for testing a specific ROM.
 */
class ExampleRomTest : public SX::Test {
protected:
    static constexpr const char* ROM_PATH = "test_rom.sfc";

    void SetUp() override {
        if (!LoadRom(ROM_PATH)) {
            GTEST_SKIP() << "Test ROM not found at: " << ROM_PATH;
        }
    }
};

// This test will be skipped if the ROM isn't present
TEST_F(ExampleRomTest, RomLoads) {
    EXPECT_TRUE(emu.IsRomLoaded());
}

TEST_F(ExampleRomTest, MemoryAccessWorks) {
    RunFrames(10);
    // Example: Check WRAM at $7E0000
    // uint8_t value = ReadByte(0x7E0000);
    // EXPECT_NE(value, 0);
}

TEST_F(ExampleRomTest, CanRunManyFrames) {
    RunFrames(600);  // 10 seconds at 60fps
    EXPECT_EQ(GetFrameCount(), 600u);
}

} // namespace
