/**
 * sxtest - Minimal ROM test
 *
 * This test uses an embedded minimal SNES ROM that displays a blue screen.
 * It verifies that the emulator can:
 * - Load a ROM from memory
 * - Run frames
 * - Access WRAM
 */

#include <sxtest.h>
#include "minimal_rom.h"

namespace {

class MinimalRomTest : public SX::Test {
protected:
    void SetUp() override {
        bool loaded = LoadRom(minimal_test_rom, minimal_test_rom_len);
        ASSERT_TRUE(loaded) << "Failed to load embedded minimal ROM";
    }
};

TEST_F(MinimalRomTest, RomLoads) {
    EXPECT_TRUE(emu.IsRomLoaded());
}

TEST_F(MinimalRomTest, CanRunFrames) {
    // Run 60 frames (1 second at 60fps)
    RunFrames(60);
    EXPECT_EQ(GetFrameCount(), 60u);
}

TEST_F(MinimalRomTest, WRAMIsAccessible) {
    // Check WRAM BEFORE running any frames
    uint8_t* wram = emu.GetWRAM();
    printf("BEFORE any frames:\n");
    if (wram) {
        printf("  WRAM[0x0000-0x0009]:");
        for (int i = 0; i < 10; i++) {
            printf(" %02X", wram[i]);
        }
        printf("\n");
    }

    // Run a few frames to let the ROM initialize
    RunFrames(10);

    // WRAM should be accessible
    EXPECT_NE(wram, nullptr) << "WRAM should be accessible";

    printf("After 10 frames:\n");
    printf("  WRAM[0x0000-0x0009]:");
    for (int i = 0; i < 10; i++) {
        printf(" %02X", wram[i]);
    }
    printf("\n");
}

TEST_F(MinimalRomTest, CanSaveAndLoadState) {
    RunFrames(30);

    // Save state
    auto state = emu.SaveState();
    EXPECT_FALSE(state.empty()) << "Save state should return data";

    // Run more frames
    RunFrames(30);
    uint64_t frames_after = GetFrameCount();

    // Load state (frame count resets to saved point)
    EXPECT_TRUE(emu.LoadState(state));
}

TEST_F(MinimalRomTest, PerformanceTest) {
    // Run 600 frames (10 seconds of emulated time)
    // This should complete very quickly in headless mode
    RunFrames(600);
    EXPECT_EQ(GetFrameCount(), 600u);
}

} // namespace
