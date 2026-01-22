/*
 * Main Entry Point - SNES Scrabble AI
 * Magpie Port
 *
 * Minimal implementation: display board, shuffle bag, draw tiles
 */

#include "snes.h"
#include "ppu.h"
#include "game.h"

/* Game state in WRAM */
static GameState game;

/* Memory locations for test harness verification */
/* These are placed at fixed addresses for sxtest to read */
#define TEST_MARKER_ADDR    ((volatile uint16_t *)0x0700)
#define TEST_BAG_COUNT_ADDR ((volatile uint8_t *)0x0702)
#define TEST_P1_RACK_ADDR   ((volatile uint8_t *)0x0710)
#define TEST_P2_RACK_ADDR   ((volatile uint8_t *)0x0718)
#define TEST_DONE_FLAG_ADDR ((volatile uint16_t *)0x0720)

#define TEST_MARKER_VALUE   0xCAFE
#define TEST_DONE_VALUE     0xBEEF

/* Write test data to fixed memory locations for verification */
static void write_test_data(void) {
    uint8_t i;

    /* Write marker to indicate game is initialized */
    *TEST_MARKER_ADDR = TEST_MARKER_VALUE;

    /* Write bag count */
    *TEST_BAG_COUNT_ADDR = game.bag.count;

    /* Write player 1 rack */
    for (i = 0; i < RACK_SIZE; i++) {
        TEST_P1_RACK_ADDR[i] = game.players[0].rack.tiles[i];
    }

    /* Write player 2 rack */
    for (i = 0; i < RACK_SIZE; i++) {
        TEST_P2_RACK_ADDR[i] = game.players[1].rack.tiles[i];
    }

    /* Write done flag */
    *TEST_DONE_FLAG_ADDR = TEST_DONE_VALUE;
}

/* Main function */
int main(void) {
    /* Initialize game with a fixed seed for reproducibility */
    game_init(&game, 0x12345678);

    /* Write test data for sxtest verification */
    write_test_data();

    /* Initialize display */
    init_game_display();

    /* Draw the board at position (1, 2) */
    draw_board(game.board, 1, 2);

    /* Draw player racks */
    draw_rack(game.players[0].rack.tiles, 18, 9, 0);
    draw_rack(game.players[1].rack.tiles, 18, 11, 1);

    /* Draw game info */
    draw_game_info(game.players[0].score, game.players[1].score, game.bag.count);

    /* Draw title */
    ppu_draw_string(1, 0, "SCRABBLE AI", 0);

    /* Enable display at full brightness */
    ppu_enable_display(15);

    /* Main loop */
    while (1) {
        /* Wait for VBlank */
        wait_vblank();
    }

    return 0;
}
