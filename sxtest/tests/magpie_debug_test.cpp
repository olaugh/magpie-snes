/**
 * Debug test for Magpie ROM
 *
 * Quick test to verify main() is executing
 */

#include <sxtest.h>
#include <cstdio>

// Include the ROM data directly
#include "magpie_rom.h"

using namespace SX::TestRoms;

class MagpieDebugTest : public SX::Test {
protected:
    void SetUp() override {
        bool loaded = LoadRom(MAGPIE_ROM, MAGPIE_ROM_SIZE);
        ASSERT_TRUE(loaded) << "Failed to load Magpie ROM";
    }
};

TEST_F(MagpieDebugTest, DebugMarker) {
    // Run many frames to ensure initialization completes
    RunFrames(300);

    // Check debug stage marker (single byte, 1-8 = progress stages)
    uint8_t stage = ReadByte(0x7E0000 + MAGPIE_DEBUG_STAGE_ADDR);
    printf("Debug stage at $%04X: 0x%02X (expected 0x08 = all done)\n", MAGPIE_DEBUG_STAGE_ADDR, stage);
    printf("  Stage meanings: 1=main started, 2=game_init, 3=test_data, 4=display_init\n");
    printf("                  5=board_drawn, 6=racks_drawn, 7=all_drawing, 8=display_enabled\n");

    // Check test marker
    uint16_t marker = ReadWord(0x7E0000 + MAGPIE_TEST_MARKER_ADDR);
    printf("Test marker at $%04X: 0x%04X (expected 0x%04X)\n", MAGPIE_TEST_MARKER_ADDR, marker, MAGPIE_TEST_MARKER_VALUE);

    // Check done flag
    uint16_t done = ReadWord(0x7E0000 + MAGPIE_DONE_FLAG_ADDR);
    printf("Done flag at $%04X: 0x%04X (expected 0x%04X)\n", MAGPIE_DONE_FLAG_ADDR, done, MAGPIE_DONE_FLAG_VALUE);

    // Check bag count
    uint8_t bag = ReadByte(0x7E0000 + MAGPIE_BAG_COUNT_ADDR);
    printf("Bag count at $%04X: %d (expected %d)\n", MAGPIE_BAG_COUNT_ADDR, bag, EXPECTED_BAG_COUNT);

    // Dump first 48 bytes of WRAM starting at test marker address
    printf("WRAM $%04X-$%04X:\n", MAGPIE_TEST_MARKER_ADDR, MAGPIE_TEST_MARKER_ADDR + 47);
    for (int i = 0; i < 48; i += 8) {
        printf("  $%04X:", MAGPIE_TEST_MARKER_ADDR + i);
        for (int j = 0; j < 8; j++) {
            printf(" %02X", ReadByte(0x7E0000 + MAGPIE_TEST_MARKER_ADDR + i + j));
        }
        printf("\n");
    }

    // Dump zero page to see if sp is initialized
    printf("\nZero page $00-$0F (includes sp):\n");
    for (int i = 0; i < 16; i += 8) {
        printf("  $%02X:", i);
        for (int j = 0; j < 8; j++) {
            printf(" %02X", ReadByte(0x7E0000 + i + j));
        }
        printf("\n");
    }

    // Verify markers are set (main ran successfully)
    EXPECT_EQ(marker, MAGPIE_TEST_MARKER_VALUE) << "Test marker should be 0xCAFE";
    EXPECT_EQ(done, MAGPIE_DONE_FLAG_VALUE) << "Done flag should be 0xBEEF";
    EXPECT_EQ(bag, EXPECTED_BAG_COUNT) << "Bag count should be 86";
}
