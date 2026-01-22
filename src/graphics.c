/*
 * PPU Graphics Driver for SNES
 * Magpie Scrabble AI Port
 *
 * Implements basic text/tile rendering using Mode 1
 */

#include "snes.h"
#include "ppu.h"

/* External frame counter (defined in vectors.s) */
extern volatile uint16_t frame_counter;

/* ----- Helper functions (moved from inline) ----- */

/* Set a single palette color */
void set_color(uint8_t index, uint16_t color) {
    REG_CGADD = index;
    REG_CGDATA = color & 0xFF;
    REG_CGDATA = color >> 8;
}

/* Clear tilemap */
void clear_tilemap(uint16_t map_addr, uint16_t tile) {
    int i;
    REG_VMAIN = VMAIN_INCREMENT_HIGH;
    REG_VMADDL = map_addr & 0xFF;
    REG_VMADDH = map_addr >> 8;
    for (i = 0; i < 32 * 32; i++) {
        REG_VMDATAL = tile & 0xFF;
        REG_VMDATAH = tile >> 8;
    }
}

/* Set tilemap entry */
void set_tile(uint16_t map_addr, uint8_t x, uint8_t y, uint16_t tile) {
    uint16_t addr = map_addr + y * 32 + x;
    REG_VMAIN = VMAIN_INCREMENT_HIGH;
    REG_VMADDL = addr & 0xFF;
    REG_VMADDH = addr >> 8;
    REG_VMDATAL = tile & 0xFF;
    REG_VMDATAH = tile >> 8;
}

/* Write a word to VRAM */
void vram_write_word(uint16_t addr, uint16_t data) {
    REG_VMAIN = VMAIN_INCREMENT_HIGH;
    REG_VMADDL = addr & 0xFF;
    REG_VMADDH = addr >> 8;
    REG_VMDATAL = data & 0xFF;
    REG_VMDATAH = data >> 8;
}

/* Write a block of words to VRAM */
void vram_write_block(uint16_t addr, const uint16_t *data, uint16_t count) {
    REG_VMAIN = VMAIN_INCREMENT_HIGH;
    REG_VMADDL = addr & 0xFF;
    REG_VMADDH = addr >> 8;
    while (count--) {
        REG_VMDATAL = *data & 0xFF;
        REG_VMDATAH = *data >> 8;
        data++;
    }
}

/* DMA transfer to VRAM */
void dma_vram(uint16_t dest, const void *src, uint16_t size) {
    /* Wait for VBlank */
    while (!(REG_RDNMI & 0x80));

    /* Set VRAM destination address */
    REG_VMAIN = VMAIN_INCREMENT_HIGH;
    REG_VMADDL = dest & 0xFF;
    REG_VMADDH = dest >> 8;

    /* Set up DMA channel 0 */
    REG_DMAP0 = 0x01;           /* A->B, increment A, 2 bytes to $2118-$2119 */
    REG_BBAD0 = 0x18;           /* B-bus address: VMDATAL */
    REG_A1T0L = (uint16_t)src & 0xFF;
    REG_A1T0H = ((uint16_t)src >> 8) & 0xFF;
    REG_A1B0 = 0x00;            /* Bank 0 (adjust if needed) */
    REG_DAS0L = size & 0xFF;
    REG_DAS0H = size >> 8;

    /* Start DMA */
    REG_MDMAEN = 0x01;
}

/* DMA transfer to CGRAM (palette) */
void dma_cgram(uint8_t dest, const void *src, uint16_t size) {
    /* Wait for VBlank */
    while (!(REG_RDNMI & 0x80));

    /* Set CGRAM destination address */
    REG_CGADD = dest;

    /* Set up DMA channel 0 */
    REG_DMAP0 = 0x00;           /* A->B, increment A, 1 byte to $2122 */
    REG_BBAD0 = 0x22;           /* B-bus address: CGDATA */
    REG_A1T0L = (uint16_t)src & 0xFF;
    REG_A1T0H = ((uint16_t)src >> 8) & 0xFF;
    REG_A1B0 = 0x00;            /* Bank 0 */
    REG_DAS0L = size & 0xFF;
    REG_DAS0H = size >> 8;

    /* Start DMA */
    REG_MDMAEN = 0x01;
}

/* Wait for VBlank (frame counter change) */
void wait_vblank(void) {
    uint16_t start = frame_counter;
    while (frame_counter == start);
}

/* Wait for NMI flag */
void wait_nmi(void) {
    while (!(REG_RDNMI & 0x80));
}

/* Read joypad 1 */
uint16_t read_joypad1(void) {
    /* Wait for auto-joypad read to finish */
    while (REG_HVBJOY & 0x01);
    return REG_JOY1;
}

/* ----- End helper functions ----- */

/* 8x8 1bpp font data (ASCII 32-127) */
/* Standard IBM VGA style font, same as Genesis version */
static const unsigned char font_data[96][8] = {
    /* ' ' */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '!' */ {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},
    /* '"' */ {0x6C, 0x6C, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '#' */ {0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00},
    /* '$' */ {0x18, 0x7E, 0xC0, 0x7C, 0x06, 0xFC, 0x18, 0x00},
    /* '%' */ {0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00},
    /* '&' */ {0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00},
    /* "'" */ {0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '(' */ {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00},
    /* ')' */ {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00},
    /* '*' */ {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},
    /* '+' */ {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},
    /* ',' */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},
    /* '-' */ {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},
    /* '.' */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},
    /* '/' */ {0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00},
    /* '0' */ {0x7C, 0xCE, 0xDE, 0xF6, 0xE6, 0xC6, 0x7C, 0x00},
    /* '1' */ {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
    /* '2' */ {0x7C, 0xC6, 0x06, 0x7C, 0xC0, 0xC0, 0xFE, 0x00},
    /* '3' */ {0xFC, 0x06, 0x06, 0x3C, 0x06, 0x06, 0xFC, 0x00},
    /* '4' */ {0x0C, 0xCC, 0xCC, 0xCC, 0xFE, 0x0C, 0x0C, 0x00},
    /* '5' */ {0xFE, 0xC0, 0xFC, 0x06, 0x06, 0xC6, 0x7C, 0x00},
    /* '6' */ {0x7C, 0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0x7C, 0x00},
    /* '7' */ {0xFE, 0x06, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x00},
    /* '8' */ {0x7C, 0xC6, 0xC6, 0x7C, 0xC6, 0xC6, 0x7C, 0x00},
    /* '9' */ {0x7C, 0xC6, 0xC6, 0x7E, 0x06, 0x06, 0x7C, 0x00},
    /* ':' */ {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},
    /* ';' */ {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30},
    /* '<' */ {0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00},
    /* '=' */ {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00},
    /* '>' */ {0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00},
    /* '?' */ {0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x00},
    /* '@' */ {0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x7C, 0x00},
    /* 'A' */ {0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0x00},
    /* 'B' */ {0xFC, 0xC6, 0xC6, 0xFC, 0xC6, 0xC6, 0xFC, 0x00},
    /* 'C' */ {0x7C, 0xC6, 0xC0, 0xC0, 0xC0, 0xC6, 0x7C, 0x00},
    /* 'D' */ {0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xCC, 0xF8, 0x00},
    /* 'E' */ {0xFE, 0xC0, 0xC0, 0xF8, 0xC0, 0xC0, 0xFE, 0x00},
    /* 'F' */ {0xFE, 0xC0, 0xC0, 0xF8, 0xC0, 0xC0, 0xC0, 0x00},
    /* 'G' */ {0x7C, 0xC6, 0xC0, 0xCE, 0xC6, 0xC6, 0x7E, 0x00},
    /* 'H' */ {0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00},
    /* 'I' */ {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
    /* 'J' */ {0x06, 0x06, 0x06, 0x06, 0xC6, 0xC6, 0x7C, 0x00},
    /* 'K' */ {0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00},
    /* 'L' */ {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFE, 0x00},
    /* 'M' */ {0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00},
    /* 'N' */ {0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00},
    /* 'O' */ {0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},
    /* 'P' */ {0xFC, 0xC6, 0xC6, 0xFC, 0xC0, 0xC0, 0xC0, 0x00},
    /* 'Q' */ {0x7C, 0xC6, 0xC6, 0xC6, 0xD6, 0xDE, 0x7C, 0x06},
    /* 'R' */ {0xFC, 0xC6, 0xC6, 0xFC, 0xD8, 0xCC, 0xC6, 0x00},
    /* 'S' */ {0x7C, 0xC6, 0xC0, 0x7C, 0x06, 0xC6, 0x7C, 0x00},
    /* 'T' */ {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
    /* 'U' */ {0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},
    /* 'V' */ {0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00},
    /* 'W' */ {0xC6, 0xC6, 0xD6, 0xFE, 0xFE, 0xEE, 0xC6, 0x00},
    /* 'X' */ {0xC6, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0xC6, 0x00},
    /* 'Y' */ {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},
    /* 'Z' */ {0xFE, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFE, 0x00},
    /* '[' */ {0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00},
    /* '\\' */ {0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00},
    /* ']' */ {0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00},
    /* '^' */ {0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00},
    /* '_' */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE},
    /* '`' */ {0x18, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 'a' */ {0x00, 0x00, 0x7C, 0x06, 0x7E, 0xC6, 0x7E, 0x00},
    /* 'b' */ {0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0xFC, 0x00},
    /* 'c' */ {0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC6, 0x7C, 0x00},
    /* 'd' */ {0x06, 0x06, 0x7E, 0xC6, 0xC6, 0xC6, 0x7E, 0x00},
    /* 'e' */ {0x00, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C, 0x00},
    /* 'f' */ {0x1C, 0x36, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x00},
    /* 'g' */ {0x00, 0x00, 0x7E, 0xC6, 0xC6, 0x7E, 0x06, 0x7C},
    /* 'h' */ {0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0xC6, 0x00},
    /* 'i' */ {0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00},
    /* 'j' */ {0x06, 0x00, 0x06, 0x06, 0x06, 0xC6, 0xC6, 0x7C},
    /* 'k' */ {0xC0, 0xC0, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0x00},
    /* 'l' */ {0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00},
    /* 'm' */ {0x00, 0x00, 0xEC, 0xFE, 0xD6, 0xD6, 0xC6, 0x00},
    /* 'n' */ {0x00, 0x00, 0xFC, 0xC6, 0xC6, 0xC6, 0xC6, 0x00},
    /* 'o' */ {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},
    /* 'p' */ {0x00, 0x00, 0xFC, 0xC6, 0xC6, 0xFC, 0xC0, 0xC0},
    /* 'q' */ {0x00, 0x00, 0x7E, 0xC6, 0xC6, 0x7E, 0x06, 0x06},
    /* 'r' */ {0x00, 0x00, 0xDC, 0xE6, 0xC0, 0xC0, 0xC0, 0x00},
    /* 's' */ {0x00, 0x00, 0x7E, 0xC0, 0x7C, 0x06, 0xFC, 0x00},
    /* 't' */ {0x30, 0x30, 0x7C, 0x30, 0x30, 0x36, 0x1C, 0x00},
    /* 'u' */ {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00},
    /* 'v' */ {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00},
    /* 'w' */ {0x00, 0x00, 0xC6, 0xD6, 0xD6, 0xFE, 0x6C, 0x00},
    /* 'x' */ {0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00},
    /* 'y' */ {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x7C},
    /* 'z' */ {0x00, 0x00, 0xFE, 0x0C, 0x38, 0x60, 0xFE, 0x00},
    /* '{' */ {0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0E, 0x00},
    /* '|' */ {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
    /* '}' */ {0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00},
    /* '~' */ {0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* DEL */ {0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00},
};

/* Initialize PPU for Mode 1 text display */
void ppu_init(void) {
    /* Force blank during setup */
    REG_INIDISP = INIDISP_BLANK;

    /* Mode 0: All BGs are 2bpp (matches our font format) */
    REG_BGMODE = BGMODE_0;

    /* BG1 tilemap at VRAM $7000 (word address), 32x32 */
    REG_BG1SC = (VRAM_BG1_MAP >> 8) | 0x00;

    /* BG1 tiles at VRAM $0000 */
    REG_BG12NBA = 0x00;

    /* Enable BG1 on main screen */
    REG_TM = 0x01;

    /* Initialize scroll to (0, 0) */
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG1VOFS = 0;
}

/* Convert 1bpp font to 2bpp SNES format and upload */
void ppu_load_font(void) {
    /* Each 2bpp tile is 8 words. Tile 32 starts at VRAM word address 32*8 = 256 */
    uint16_t vram_addr = VRAM_BG1_TILES + 32 * 8;
    int i, row;

    /* Force blank during VRAM writes */
    REG_INIDISP = INIDISP_BLANK;

    /* VRAM increment on high byte write */
    REG_VMAIN = VMAIN_INCREMENT_HIGH;

    for (i = 0; i < 96; i++) {
        /* Set VRAM address for this tile */
        REG_VMADDL = (vram_addr + i * 8) & 0xFF;
        REG_VMADDH = (vram_addr + i * 8) >> 8;

        for (row = 0; row < 8; row++) {
            unsigned char bits = font_data[i][row];

            /* 2bpp format: low byte = bitplane 0, high byte = bitplane 1 */
            /* For 2-color (palette 0,1): both planes same = solid color */
            REG_VMDATAL = bits;   /* Bitplane 0 */
            REG_VMDATAH = bits;   /* Bitplane 1 (same = color 3 where set) */
        }
    }

    /* Clear tile 0 (fully transparent) */
    REG_VMADDL = VRAM_BG1_TILES & 0xFF;
    REG_VMADDH = VRAM_BG1_TILES >> 8;
    for (row = 0; row < 8; row++) {
        REG_VMDATAL = 0;
        REG_VMDATAH = 0;
    }
}

/* Set up default palette */
static void setup_palette(void) {
    /* Palette 0: Text colors */
    set_color(0, COLOR_BLACK);      /* Background (transparent) */
    set_color(1, COLOR_WHITE);      /* Color 1 */
    set_color(2, COLOR_WHITE);      /* Color 2 */
    set_color(3, COLOR_WHITE);      /* Foreground (both planes set) */

    /* Palette 1: Board colors */
    set_color(4, COLOR_BLACK);
    set_color(5, COLOR_BOARD_BG);
    set_color(6, COLOR_GRIDLINE);
    set_color(7, COLOR_WHITE);

    /* Background color (color 0 of palette 0) */
    set_color(0, RGB15(0, 0, 8));   /* Dark blue background */
}

/* Draw a character at tile position */
void ppu_draw_char(uint8_t x, uint8_t y, char c, uint8_t pal) {
    uint16_t tile;
    uint16_t addr = VRAM_BG1_MAP + y * 32 + x;

    if (c >= 32 && c < 128) {
        tile = c;  /* ASCII maps directly to tile number */
    } else {
        tile = 0;  /* Blank for invalid chars */
    }

    /* Add palette bits */
    tile |= TILE_PAL(pal);

    /* Write to tilemap */
    REG_VMAIN = VMAIN_INCREMENT_HIGH;
    REG_VMADDL = addr & 0xFF;
    REG_VMADDH = addr >> 8;
    REG_VMDATAL = tile & 0xFF;
    REG_VMDATAH = tile >> 8;
}

/* Draw a string */
void ppu_draw_string(uint8_t x, uint8_t y, const char *str, uint8_t pal) {
    while (*str) {
        ppu_draw_char(x++, y, *str++, pal);
        if (x >= 32) break;  /* Wrap protection */
    }
}

/* Draw a number (simple implementation) */
void ppu_draw_number(uint8_t x, uint8_t y, int16_t num, uint8_t pal) {
    char buf[8];
    int pos = 0;
    int neg = 0;

    if (num < 0) {
        neg = 1;
        num = -num;
    }

    /* Convert to string (reversed) */
    if (num == 0) {
        buf[pos++] = '0';
    } else {
        while (num > 0 && pos < 6) {
            buf[pos++] = '0' + (num % 10);
            num /= 10;
        }
    }

    /* Draw negative sign */
    if (neg) {
        ppu_draw_char(x++, y, '-', pal);
    }

    /* Draw digits in reverse order */
    while (pos > 0) {
        ppu_draw_char(x++, y, buf[--pos], pal);
    }
}

/* Enable display */
void ppu_enable_display(uint8_t brightness) {
    REG_INIDISP = brightness & 0x0F;
}

/* Disable display (force blank) */
void ppu_disable_display(void) {
    REG_INIDISP = INIDISP_BLANK;
}

/* Clear the tilemap */
static void clear_screen(void) {
    clear_tilemap(VRAM_BG1_MAP, 0);
}

/* Test pattern display */
void display_test_pattern(void) {
    /* Initialize PPU */
    ppu_init();

    /* Set up palette */
    setup_palette();

    /* Load font */
    ppu_load_font();

    /* Clear screen */
    clear_screen();

    /* Draw test text */
    ppu_draw_string(2, 2, "SNES SCRABBLE AI", 0);
    ppu_draw_string(2, 4, "Magpie Port", 0);
    ppu_draw_string(2, 6, "Phase 1: Bootstrap", 0);

    ppu_draw_string(2, 10, "Press START", 0);

    /* Enable display at full brightness */
    ppu_enable_display(15);
}

/* Initialize display for game screen */
void init_game_display(void) {
    /* Initialize PPU */
    ppu_init();

    /* Set up palette */
    setup_palette();

    /* Load font */
    ppu_load_font();

    /* Clear screen */
    clear_screen();
}

/* Draw a single board square at screen position */
static void draw_board_square(uint8_t sx, uint8_t sy, uint8_t tile, uint8_t bonus) {
    char c;

    if (tile != 0) {
        /* Tile placed - show letter */
        if (tile >= 1 && tile <= 26) {
            c = 'A' + (tile - 1);
        } else if (tile == 27) {
            c = '?';  /* Blank */
        } else {
            c = ' ';
        }
        ppu_draw_char(sx, sy, c, 0);  /* White text */
    } else {
        /* Empty square - show bonus indicator */
        switch (bonus) {
            case 1: c = '.'; break;  /* DLS */
            case 2: c = ':'; break;  /* TLS */
            case 3: c = '='; break;  /* DWS */
            case 4: c = '#'; break;  /* TWS */
            case 5: c = '*'; break;  /* Center */
            default: c = '.'; break; /* Normal */
        }
        ppu_draw_char(sx, sy, c, 0);
    }
}

/* Draw the 15x15 board starting at screen position (bx, by) */
void draw_board(const void *board_ptr, uint8_t bx, uint8_t by) {
    const uint8_t *board = (const uint8_t *)board_ptr;
    uint8_t x, y;
    uint16_t idx;

    for (y = 0; y < 15; y++) {
        for (x = 0; x < 15; x++) {
            idx = y * 15 + x;
            /* Each Square is 2 bytes: tile, bonus */
            draw_board_square(bx + x, by + y,
                            board[idx * 2],      /* tile */
                            board[idx * 2 + 1]); /* bonus */
        }
    }
}

/* Draw player rack */
void draw_rack(const uint8_t *rack, uint8_t rx, uint8_t ry, uint8_t player) {
    uint8_t i;
    char c;
    char label[12];

    /* Draw player label */
    label[0] = 'P';
    label[1] = '1' + player;
    label[2] = ':';
    label[3] = ' ';
    label[4] = '\0';
    ppu_draw_string(rx, ry, label, 0);

    /* Draw rack tiles */
    for (i = 0; i < 7; i++) {
        if (rack[i] != 0) {
            if (rack[i] >= 1 && rack[i] <= 26) {
                c = 'A' + (rack[i] - 1);
            } else if (rack[i] == 27) {
                c = '?';
            } else {
                c = ' ';
            }
        } else {
            c = '_';  /* Empty slot */
        }
        ppu_draw_char(rx + 4 + i, ry, c, 0);
    }
}

/* Draw game info (scores, tiles remaining) */
void draw_game_info(int16_t score1, int16_t score2, uint8_t tiles_left) {
    ppu_draw_string(18, 2, "SCORE", 0);
    ppu_draw_string(18, 3, "P1:", 0);
    ppu_draw_number(22, 3, score1, 0);
    ppu_draw_string(18, 4, "P2:", 0);
    ppu_draw_number(22, 4, score2, 0);

    ppu_draw_string(18, 6, "BAG:", 0);
    ppu_draw_number(22, 6, tiles_left, 0);
}
