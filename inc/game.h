/*
 * Game State Definitions
 * Magpie Scrabble AI - SNES Port
 *
 * Core game data structures for Scrabble:
 * - Tile bag with standard distribution
 * - Player racks
 * - Board state
 */

#ifndef GAME_H
#define GAME_H

#include <stdint.h>

/* Board dimensions */
#define BOARD_SIZE      15
#define BOARD_SQUARES   (BOARD_SIZE * BOARD_SIZE)

/* Rack size */
#define RACK_SIZE       7

/* Number of players */
#define NUM_PLAYERS     2

/* Total tiles in standard English Scrabble */
#define TOTAL_TILES     100

/* Tile values (A-Z, with ? for blank) */
#define TILE_EMPTY      0       /* Empty square / no tile */
#define TILE_A          1
#define TILE_B          2
#define TILE_C          3
#define TILE_D          4
#define TILE_E          5
#define TILE_F          6
#define TILE_G          7
#define TILE_H          8
#define TILE_I          9
#define TILE_J          10
#define TILE_K          11
#define TILE_L          12
#define TILE_M          13
#define TILE_N          14
#define TILE_O          15
#define TILE_P          16
#define TILE_Q          17
#define TILE_R          18
#define TILE_S          19
#define TILE_T          20
#define TILE_U          21
#define TILE_V          22
#define TILE_W          23
#define TILE_X          24
#define TILE_Y          25
#define TILE_Z          26
#define TILE_BLANK      27      /* Blank tile */

/* Square bonus types */
#define BONUS_NONE      0
#define BONUS_DLS       1       /* Double Letter Score */
#define BONUS_TLS       2       /* Triple Letter Score */
#define BONUS_DWS       3       /* Double Word Score */
#define BONUS_TWS       4       /* Triple Word Score */
#define BONUS_CENTER    5       /* Center star (also DWS) */

/* Point values for each tile (index = tile value) */
extern const uint8_t TILE_POINTS[28];

/* Standard Scrabble tile distribution (index = tile value, value = count) */
extern const uint8_t TILE_DISTRIBUTION[28];

/* Tile bag structure */
typedef struct {
    uint8_t tiles[TOTAL_TILES]; /* Tiles in the bag */
    uint8_t count;              /* Number of tiles remaining */
} TileBag;

/* Player rack */
typedef struct {
    uint8_t tiles[RACK_SIZE];   /* Tiles on rack (TILE_EMPTY if slot empty) */
    uint8_t count;              /* Number of tiles on rack */
} Rack;

/* Player state */
typedef struct {
    Rack rack;
    int16_t score;
} Player;

/* Board square */
typedef struct {
    uint8_t tile;               /* Tile on this square (TILE_EMPTY if none) */
    uint8_t bonus;              /* Bonus type (used before tile placed) */
} Square;

/* Game state */
typedef struct {
    Square board[BOARD_SQUARES];
    TileBag bag;
    Player players[NUM_PLAYERS];
    uint8_t current_player;     /* 0 or 1 */
    uint8_t game_over;
    uint16_t turn_count;
} GameState;

/* Random number generator state */
typedef struct {
    uint32_t seed;
} RNG;

/* Function declarations */

/* Initialize RNG with seed */
void rng_init(RNG *rng, uint32_t seed);

/* Get next random number (0-65535) */
uint16_t rng_next(RNG *rng);

/* Get random number in range [0, max) */
uint16_t rng_range(RNG *rng, uint16_t max);

/* Initialize tile bag with standard distribution */
void bag_init(TileBag *bag);

/* Shuffle the tile bag using Fisher-Yates */
void bag_shuffle(TileBag *bag, RNG *rng);

/* Draw a tile from the bag (returns TILE_EMPTY if bag empty) */
uint8_t bag_draw(TileBag *bag);

/* Draw tiles to fill a rack */
void rack_fill(Rack *rack, TileBag *bag);

/* Initialize board with bonus squares */
void board_init(Square *board);

/* Initialize full game state */
void game_init(GameState *game, uint32_t seed);

/* Get tile character for display ('A'-'Z', '?' for blank, ' ' for empty) */
char tile_to_char(uint8_t tile);

/* Convert character to tile value */
uint8_t char_to_tile(char c);

#endif /* GAME_H */
