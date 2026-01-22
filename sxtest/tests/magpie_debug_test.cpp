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

    // Check debug stage marker at $0730 (single byte, 1-8 = progress stages)
    uint8_t stage = ReadByte(0x7E0730);
    printf("Debug stage at $0730: 0x%02X (expected 0x08 = all done)\n", stage);
    printf("  Stage meanings: 1=main started, 2=game_init, 3=test_data, 4=display_init\n");
    printf("                  5=board_drawn, 6=racks_drawn, 7=all_drawing, 8=display_enabled\n");

    // Check test marker at $0700
    uint16_t marker = ReadWord(0x7E0700);
    printf("Test marker at $0700: 0x%04X (expected 0xCAFE)\n", marker);

    // Check done flag at $0720
    uint16_t done = ReadWord(0x7E0720);
    printf("Done flag at $0720: 0x%04X (expected 0xBEEF)\n", done);

    // Check bag count at $0702
    uint8_t bag = ReadByte(0x7E0702);
    printf("Bag count at $0702: %d (expected 86)\n", bag);

    // Dump first 48 bytes of WRAM at $0700
    printf("WRAM $0700-$072F:\n");
    for (int i = 0; i < 48; i += 8) {
        printf("  $%04X:", 0x0700 + i);
        for (int j = 0; j < 8; j++) {
            printf(" %02X", ReadByte(0x7E0700 + i + j));
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
    EXPECT_EQ(marker, 0xCAFE) << "Test marker should be 0xCAFE";
    EXPECT_EQ(done, 0xBEEF) << "Done flag should be 0xBEEF";
    EXPECT_EQ(bag, 86) << "Bag count should be 86";
}
