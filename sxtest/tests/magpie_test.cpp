/**
 * sxtest - Magpie Scrabble ROM test
 *
 * This test verifies that the SNES emulator correctly executes
 * the Magpie Scrabble minimal implementation:
 * - Board initialization
 * - Bag shuffling with PRNG
 * - Drawing tiles to player racks
 */

#include <sxtest.h>
#include "magpie_rom.h"

namespace {

using namespace SX::TestRoms;

class MagpieTest : public SX::Test {
protected:
    void SetUp() override {
        bool loaded = LoadRom(MAGPIE_ROM, MAGPIE_ROM_SIZE);
        ASSERT_TRUE(loaded) << "Failed to load Magpie ROM";
    }
};

TEST_F(MagpieTest, RomLoads) {
    EXPECT_TRUE(emu.IsRomLoaded());
}

TEST_F(MagpieTest, WRAMIsAccessible) {
    RunFrames(10);
    uint8_t* wram = emu.GetWRAM();
    EXPECT_NE(wram, nullptr) << "WRAM should be accessible";
}

TEST_F(MagpieTest, GameInitializes) {
    // Run until done flag is set or timeout
    int frame = emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    ASSERT_GE(frame, 0) << "Timed out waiting for game initialization";

    // Verify test marker
    uint16_t marker = ReadWord(0x7E0000 + MAGPIE_TEST_MARKER_ADDR);
    EXPECT_EQ(marker, MAGPIE_TEST_MARKER_VALUE) << "Test marker should be 0xCAFE";

    // Verify done flag
    uint16_t done = ReadWord(0x7E0000 + MAGPIE_DONE_FLAG_ADDR);
    EXPECT_EQ(done, MAGPIE_DONE_FLAG_VALUE) << "Done flag should be 0xBEEF";
}

TEST_F(MagpieTest, BagCountCorrect) {
    // Run until initialized
    emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    // Bag should have 86 tiles remaining (100 - 14 drawn)
    uint8_t bag_count = ReadByte(0x7E0000 + MAGPIE_BAG_COUNT_ADDR);
    EXPECT_EQ(bag_count, EXPECTED_BAG_COUNT)
        << "Bag should have 86 tiles after drawing 14 (7 per player)";
}

TEST_F(MagpieTest, Player1RackHas7Tiles) {
    // Run until initialized
    emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    // Check player 1 rack has 7 valid tiles
    int tile_count = 0;
    for (int i = 0; i < RACK_SIZE; i++) {
        uint8_t tile = ReadByte(0x7E0000 + MAGPIE_P1_RACK_ADDR + i);
        if (tile >= TILE_A && tile <= TILE_BLANK) {
            tile_count++;
        }
    }
    EXPECT_EQ(tile_count, RACK_SIZE) << "Player 1 should have 7 tiles";
}

TEST_F(MagpieTest, Player2RackHas7Tiles) {
    // Run until initialized
    emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    // Check player 2 rack has 7 valid tiles
    int tile_count = 0;
    for (int i = 0; i < RACK_SIZE; i++) {
        uint8_t tile = ReadByte(0x7E0000 + MAGPIE_P2_RACK_ADDR + i);
        if (tile >= TILE_A && tile <= TILE_BLANK) {
            tile_count++;
        }
    }
    EXPECT_EQ(tile_count, RACK_SIZE) << "Player 2 should have 7 tiles";
}

TEST_F(MagpieTest, AllTilesAreValid) {
    // Run until initialized
    emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    // Check all tiles in both racks are valid (1-27)
    for (int i = 0; i < RACK_SIZE; i++) {
        uint8_t tile1 = ReadByte(0x7E0000 + MAGPIE_P1_RACK_ADDR + i);
        EXPECT_GE(tile1, TILE_A) << "P1 tile " << i << " should be >= 1";
        EXPECT_LE(tile1, TILE_BLANK) << "P1 tile " << i << " should be <= 27";

        uint8_t tile2 = ReadByte(0x7E0000 + MAGPIE_P2_RACK_ADDR + i);
        EXPECT_GE(tile2, TILE_A) << "P2 tile " << i << " should be >= 1";
        EXPECT_LE(tile2, TILE_BLANK) << "P2 tile " << i << " should be <= 27";
    }
}

TEST_F(MagpieTest, TotalTileCountIsCorrect) {
    // Run until initialized
    emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    // Total should be: bag (86) + P1 rack (7) + P2 rack (7) = 100
    uint8_t bag_count = ReadByte(0x7E0000 + MAGPIE_BAG_COUNT_ADDR);
    int total = bag_count + RACK_SIZE * 2;

    EXPECT_EQ(total, TOTAL_TILES)
        << "Total tiles (bag + racks) should equal 100";
}

TEST_F(MagpieTest, Performance) {
    // Game should initialize quickly
    int frames = emu.RunUntilMemoryEquals(
        0x7E0000 + MAGPIE_DONE_FLAG_ADDR,
        MAGPIE_DONE_FLAG_VALUE & 0xFF,
        300
    );

    ASSERT_GE(frames, 0) << "Timed out";
    EXPECT_LT(frames, 60) << "Game should initialize within 1 second (60 frames)";
}

} // namespace
