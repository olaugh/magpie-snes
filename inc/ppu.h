/*
 * PPU (Picture Processing Unit) Definitions and Helpers
 * Magpie Scrabble AI - SNES Port
 */

#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include "snes.h"

/* VRAM Layout (words, not bytes) */
#define VRAM_BG1_TILES      0x0000  /* BG1 tile data */
#define VRAM_BG2_TILES      0x4000  /* BG2 tile data */
#define VRAM_BG1_MAP        0x7000  /* BG1 tilemap (32x32 = 2KB = $400 words) */
#define VRAM_BG2_MAP        0x7400  /* BG2 tilemap */
#define VRAM_OBJ_TILES      0x6000  /* Sprite tile data */

/* Tilemap entry format */
#define TILE_CHAR(n)        ((n) & 0x3FF)   /* Character number (0-1023) */
#define TILE_PAL(n)         (((n) & 0x07) << 10)  /* Palette (0-7) */
#define TILE_PRIORITY       0x2000          /* Priority bit */
#define TILE_HFLIP          0x4000          /* Horizontal flip */
#define TILE_VFLIP          0x8000          /* Vertical flip */

/* Color format: 15-bit BGR */
#define RGB15(r,g,b)        (((b) << 10) | ((g) << 5) | (r))

/* Common colors */
#define COLOR_BLACK         RGB15(0, 0, 0)
#define COLOR_WHITE         RGB15(31, 31, 31)
#define COLOR_RED           RGB15(31, 0, 0)
#define COLOR_GREEN         RGB15(0, 31, 0)
#define COLOR_BLUE          RGB15(0, 0, 31)
#define COLOR_YELLOW        RGB15(31, 31, 0)
#define COLOR_CYAN          RGB15(0, 31, 31)
#define COLOR_MAGENTA       RGB15(31, 0, 31)
#define COLOR_GRAY          RGB15(16, 16, 16)
#define COLOR_DARKGRAY      RGB15(8, 8, 8)

/* Board colors (adapted from Genesis version) */
#define COLOR_BOARD_BG      RGB15(8, 8, 8)      /* Dark gray board background */
#define COLOR_GRIDLINE      RGB15(4, 4, 4)      /* Grid lines */
#define COLOR_TWS           RGB15(31, 8, 4)     /* Triple word - red */
#define COLOR_DWS           RGB15(20, 12, 16)   /* Double word - pink */
#define COLOR_TLS           RGB15(8, 8, 20)     /* Triple letter - dark blue */
#define COLOR_DLS           RGB15(16, 16, 24)   /* Double letter - light blue */
#define COLOR_TILE_BG       RGB15(26, 24, 16)   /* Letter tile cream */
#define COLOR_TILE_FG       RGB15(0, 0, 0)      /* Letter tile foreground */
#define COLOR_BLANK_BG      RGB15(20, 10, 20)   /* Blank tile purple */

/* Function declarations - implementations in graphics.c */

/* DMA transfer to VRAM */
void dma_vram(uint16_t dest, const void *src, uint16_t size);

/* DMA transfer to CGRAM (palette) */
void dma_cgram(uint8_t dest, const void *src, uint16_t size);

/* Write a word to VRAM */
void vram_write_word(uint16_t addr, uint16_t data);

/* Write a block of words to VRAM (slow, use DMA for large transfers) */
void vram_write_block(uint16_t addr, const uint16_t *data, uint16_t count);

/* Set tilemap entry */
void set_tile(uint16_t map_addr, uint8_t x, uint8_t y, uint16_t tile);

/* Clear tilemap */
void clear_tilemap(uint16_t map_addr, uint16_t tile);

/* Initialize PPU for text/tile mode */
void ppu_init(void);

/* Load font tiles */
void ppu_load_font(void);

/* Draw a character */
void ppu_draw_char(uint8_t x, uint8_t y, char c, uint8_t pal);

/* Draw a string */
void ppu_draw_string(uint8_t x, uint8_t y, const char *str, uint8_t pal);

/* Draw a number */
void ppu_draw_number(uint8_t x, uint8_t y, int16_t num, uint8_t pal);

/* Turn on display */
void ppu_enable_display(uint8_t brightness);

/* Force blank */
void ppu_disable_display(void);

/* Initialize game display */
void init_game_display(void);

/* Draw the 15x15 Scrabble board */
void draw_board(const void *board_ptr, uint8_t bx, uint8_t by);

/* Draw a player's rack */
void draw_rack(const uint8_t *rack, uint8_t rx, uint8_t ry, uint8_t player);

/* Draw game info (scores and bag count) */
void draw_game_info(int16_t score1, int16_t score2, uint8_t tiles_left);

#endif /* PPU_H */
