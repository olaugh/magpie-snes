/*
 * SNES Hardware Definitions for C
 * Magpie Scrabble AI - SNES Port
 */

#ifndef SNES_H
#define SNES_H

#include <stdint.h>

/* PPU Registers ($2100-$213F) */
#define REG_INIDISP     (*(volatile uint8_t *)0x2100)
#define REG_OBSEL       (*(volatile uint8_t *)0x2101)
#define REG_OAMADDL     (*(volatile uint8_t *)0x2102)
#define REG_OAMADDH     (*(volatile uint8_t *)0x2103)
#define REG_OAMDATA     (*(volatile uint8_t *)0x2104)
#define REG_BGMODE      (*(volatile uint8_t *)0x2105)
#define REG_MOSAIC      (*(volatile uint8_t *)0x2106)
#define REG_BG1SC       (*(volatile uint8_t *)0x2107)
#define REG_BG2SC       (*(volatile uint8_t *)0x2108)
#define REG_BG3SC       (*(volatile uint8_t *)0x2109)
#define REG_BG4SC       (*(volatile uint8_t *)0x210A)
#define REG_BG12NBA     (*(volatile uint8_t *)0x210B)
#define REG_BG34NBA     (*(volatile uint8_t *)0x210C)
#define REG_BG1HOFS     (*(volatile uint8_t *)0x210D)
#define REG_BG1VOFS     (*(volatile uint8_t *)0x210E)
#define REG_BG2HOFS     (*(volatile uint8_t *)0x210F)
#define REG_BG2VOFS     (*(volatile uint8_t *)0x2110)
#define REG_BG3HOFS     (*(volatile uint8_t *)0x2111)
#define REG_BG3VOFS     (*(volatile uint8_t *)0x2112)
#define REG_BG4HOFS     (*(volatile uint8_t *)0x2113)
#define REG_BG4VOFS     (*(volatile uint8_t *)0x2114)
#define REG_VMAIN       (*(volatile uint8_t *)0x2115)
#define REG_VMADDL      (*(volatile uint8_t *)0x2116)
#define REG_VMADDH      (*(volatile uint8_t *)0x2117)
#define REG_VMDATAL     (*(volatile uint8_t *)0x2118)
#define REG_VMDATAH     (*(volatile uint8_t *)0x2119)
#define REG_CGADD       (*(volatile uint8_t *)0x2121)
#define REG_CGDATA      (*(volatile uint8_t *)0x2122)
#define REG_W12SEL      (*(volatile uint8_t *)0x2123)
#define REG_W34SEL      (*(volatile uint8_t *)0x2124)
#define REG_WOBJSEL     (*(volatile uint8_t *)0x2125)
#define REG_TM          (*(volatile uint8_t *)0x212C)
#define REG_TS          (*(volatile uint8_t *)0x212D)
#define REG_CGWSEL      (*(volatile uint8_t *)0x2130)
#define REG_CGADSUB     (*(volatile uint8_t *)0x2131)
#define REG_COLDATA     (*(volatile uint8_t *)0x2132)
#define REG_SETINI      (*(volatile uint8_t *)0x2133)

/* PPU Status Registers (read) */
#define REG_RDNMI       (*(volatile uint8_t *)0x4210)
#define REG_TIMEUP      (*(volatile uint8_t *)0x4211)
#define REG_HVBJOY      (*(volatile uint8_t *)0x4212)

/* CPU Control Registers */
#define REG_NMITIMEN    (*(volatile uint8_t *)0x4200)
#define REG_WRIO        (*(volatile uint8_t *)0x4201)
#define REG_WRMPYA      (*(volatile uint8_t *)0x4202)
#define REG_WRMPYB      (*(volatile uint8_t *)0x4203)
#define REG_WRDIVL      (*(volatile uint8_t *)0x4204)
#define REG_WRDIVH      (*(volatile uint8_t *)0x4205)
#define REG_WRDIVB      (*(volatile uint8_t *)0x4206)
#define REG_MDMAEN      (*(volatile uint8_t *)0x420B)
#define REG_HDMAEN      (*(volatile uint8_t *)0x420C)
#define REG_MEMSEL      (*(volatile uint8_t *)0x420D)

/* Math Result Registers (read) */
#define REG_RDDIVL      (*(volatile uint8_t *)0x4214)
#define REG_RDDIVH      (*(volatile uint8_t *)0x4215)
#define REG_RDMPYL      (*(volatile uint8_t *)0x4216)
#define REG_RDMPYH      (*(volatile uint8_t *)0x4217)

/* Joypad Registers */
#define REG_JOY1L       (*(volatile uint8_t *)0x4218)
#define REG_JOY1H       (*(volatile uint8_t *)0x4219)
#define REG_JOY2L       (*(volatile uint8_t *)0x421A)
#define REG_JOY2H       (*(volatile uint8_t *)0x421B)
#define REG_JOY1        (*(volatile uint16_t *)0x4218)
#define REG_JOY2        (*(volatile uint16_t *)0x421A)

/* WRAM Access */
#define REG_WMDATA      (*(volatile uint8_t *)0x2180)
#define REG_WMADDL      (*(volatile uint8_t *)0x2181)
#define REG_WMADDM      (*(volatile uint8_t *)0x2182)
#define REG_WMADDH      (*(volatile uint8_t *)0x2183)

/* DMA Registers (channel 0) */
#define REG_DMAP0       (*(volatile uint8_t *)0x4300)
#define REG_BBAD0       (*(volatile uint8_t *)0x4301)
#define REG_A1T0L       (*(volatile uint8_t *)0x4302)
#define REG_A1T0H       (*(volatile uint8_t *)0x4303)
#define REG_A1B0        (*(volatile uint8_t *)0x4304)
#define REG_DAS0L       (*(volatile uint8_t *)0x4305)
#define REG_DAS0H       (*(volatile uint8_t *)0x4306)

/* Joypad Button Masks */
#define JOY_B           0x8000
#define JOY_Y           0x4000
#define JOY_SELECT      0x2000
#define JOY_START       0x1000
#define JOY_UP          0x0800
#define JOY_DOWN        0x0400
#define JOY_LEFT        0x0200
#define JOY_RIGHT       0x0100
#define JOY_A           0x0080
#define JOY_X           0x0040
#define JOY_L           0x0020
#define JOY_R           0x0010

/* INIDISP values */
#define INIDISP_BLANK   0x80    /* Force blank (screen off) */
#define INIDISP_ON(b)   (b)     /* Screen on with brightness b (0-15) */

/* BGMODE values */
#define BGMODE_0        0x00    /* Mode 0: 4 x 2bpp */
#define BGMODE_1        0x01    /* Mode 1: 2 x 4bpp + 1 x 2bpp */
#define BGMODE_2        0x02    /* Mode 2: 2 x 4bpp (offset-per-tile) */
#define BGMODE_3        0x03    /* Mode 3: 1 x 8bpp + 1 x 4bpp */
#define BGMODE_7        0x07    /* Mode 7: 1 x 8bpp (rotation/scaling) */

/* VMAIN values */
#define VMAIN_INCREMENT_LOW     0x00    /* Increment on low byte write */
#define VMAIN_INCREMENT_HIGH    0x80    /* Increment on high byte write */
#define VMAIN_INCREMENT_1       0x00    /* Increment by 1 word */
#define VMAIN_INCREMENT_32      0x01    /* Increment by 32 words */
#define VMAIN_INCREMENT_128     0x02    /* Increment by 128 words */

/* Utility macros */
#define VRAM_ADDR(addr)     do { REG_VMADDL = (addr) & 0xFF; REG_VMADDH = (addr) >> 8; } while(0)
#define VRAM_WRITE(data)    do { REG_VMDATAL = (data) & 0xFF; REG_VMDATAH = (data) >> 8; } while(0)
#define CGRAM_ADDR(addr)    REG_CGADD = (addr)
#define CGRAM_WRITE(col)    do { REG_CGDATA = (col) & 0xFF; REG_CGDATA = (col) >> 8; } while(0)

/* Frame counter (incremented in NMI) */
extern volatile uint16_t frame_counter;

/* VBlank callback */
extern void (*vblank_handler)(void);

/* Wait for VBlank - implemented in main.c */
void wait_vblank(void);

/* Wait for NMI flag clear (beginning of VBlank) */
void wait_nmi(void);

/* Read joypad 1 */
uint16_t read_joypad1(void);

/* Set a single palette color */
void set_color(uint8_t index, uint16_t color);

#endif /* SNES_H */
