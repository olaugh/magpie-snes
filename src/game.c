/*
 * Game Logic Implementation
 * Magpie Scrabble AI - SNES Port
 *
 * Implements core Scrabble game mechanics:
 * - Tile bag management
 * - Random number generation
 * - Board initialization
 */

#include "game.h"

/* Point values for each tile (0=empty, 1-26=A-Z, 27=blank) */
const uint8_t TILE_POINTS[28] = {
    0,  /* TILE_EMPTY */
    1,  /* A */
    3,  /* B */
    3,  /* C */
    2,  /* D */
    1,  /* E */
    4,  /* F */
    2,  /* G */
    4,  /* H */
    1,  /* I */
    8,  /* J */
    5,  /* K */
    1,  /* L */
    3,  /* M */
    1,  /* N */
    1,  /* O */
    3,  /* P */
    10, /* Q */
    1,  /* R */
    1,  /* S */
    1,  /* T */
    1,  /* U */
    4,  /* V */
    4,  /* W */
    8,  /* X */
    4,  /* Y */
    10, /* Z */
    0   /* BLANK */
};

/* Standard Scrabble tile distribution */
const uint8_t TILE_DISTRIBUTION[28] = {
    0,  /* TILE_EMPTY - not in bag */
    9,  /* A */
    2,  /* B */
    2,  /* C */
    4,  /* D */
    12, /* E */
    2,  /* F */
    3,  /* G */
    2,  /* H */
    9,  /* I */
    1,  /* J */
    1,  /* K */
    4,  /* L */
    2,  /* M */
    6,  /* N */
    8,  /* O */
    2,  /* P */
    1,  /* Q */
    6,  /* R */
    4,  /* S */
    6,  /* T */
    4,  /* U */
    2,  /* V */
    2,  /* W */
    1,  /* X */
    2,  /* Y */
    1,  /* Z */
    2   /* BLANK */
};

/* Board bonus layout (15x15) - standard Scrabble board */
/* Uses BONUS_* constants: 0=none, 1=DLS, 2=TLS, 3=DWS, 4=TWS, 5=center */
static const uint8_t BOARD_BONUSES[BOARD_SQUARES] = {
    /* Row 0 */  4,0,0,1,0,0,0,4,0,0,0,1,0,0,4,
    /* Row 1 */  0,3,0,0,0,2,0,0,0,2,0,0,0,3,0,
    /* Row 2 */  0,0,3,0,0,0,1,0,1,0,0,0,3,0,0,
    /* Row 3 */  1,0,0,3,0,0,0,1,0,0,0,3,0,0,1,
    /* Row 4 */  0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,
    /* Row 5 */  0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,
    /* Row 6 */  0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,
    /* Row 7 */  4,0,0,1,0,0,0,5,0,0,0,1,0,0,4,
    /* Row 8 */  0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,
    /* Row 9 */  0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,
    /* Row 10 */ 0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,
    /* Row 11 */ 1,0,0,3,0,0,0,1,0,0,0,3,0,0,1,
    /* Row 12 */ 0,0,3,0,0,0,1,0,1,0,0,0,3,0,0,
    /* Row 13 */ 0,3,0,0,0,2,0,0,0,2,0,0,0,3,0,
    /* Row 14 */ 4,0,0,1,0,0,0,4,0,0,0,1,0,0,4
};

/*
 * Linear Congruential Generator (LCG)
 * Uses parameters from Numerical Recipes
 * a = 1664525, c = 1013904223, m = 2^32
 */
void rng_init(RNG *rng, uint32_t seed) {
    rng->seed = seed;
    if (rng->seed == 0) {
        rng->seed = 1;  /* Avoid zero seed */
    }
}

uint16_t rng_next(RNG *rng) {
    /* LCG formula: seed = (a * seed + c) mod m */
    rng->seed = rng->seed * 1664525UL + 1013904223UL;
    /* Return high 16 bits (better quality) */
    return (uint16_t)(rng->seed >> 16);
}

uint16_t rng_range(RNG *rng, uint16_t max) {
    if (max == 0) return 0;
    return rng_next(rng) % max;
}

/*
 * Initialize tile bag with standard Scrabble distribution
 */
void bag_init(TileBag *bag) {
    uint8_t idx = 0;
    uint8_t tile, count;

    for (tile = 1; tile <= TILE_BLANK; tile++) {
        count = TILE_DISTRIBUTION[tile];
        while (count > 0) {
            bag->tiles[idx++] = tile;
            count--;
        }
    }
    bag->count = idx;  /* Should be TOTAL_TILES (100) */
}

/*
 * Fisher-Yates shuffle
 */
void bag_shuffle(TileBag *bag, RNG *rng) {
    uint8_t i, j, temp;

    for (i = bag->count - 1; i > 0; i--) {
        j = rng_range(rng, i + 1);
        /* Swap tiles[i] and tiles[j] */
        temp = bag->tiles[i];
        bag->tiles[i] = bag->tiles[j];
        bag->tiles[j] = temp;
    }
}

/*
 * Draw a tile from the bag
 */
uint8_t bag_draw(TileBag *bag) {
    if (bag->count == 0) {
        return TILE_EMPTY;
    }
    bag->count--;
    return bag->tiles[bag->count];
}

/*
 * Fill rack with tiles from bag
 */
void rack_fill(Rack *rack, TileBag *bag) {
    uint8_t i;

    for (i = 0; i < RACK_SIZE; i++) {
        if (rack->tiles[i] == TILE_EMPTY && bag->count > 0) {
            rack->tiles[i] = bag_draw(bag);
            rack->count++;
        }
    }
}

/*
 * Initialize board with bonus squares
 */
void board_init(Square *board) {
    uint16_t i;

    for (i = 0; i < BOARD_SQUARES; i++) {
        board[i].tile = TILE_EMPTY;
        board[i].bonus = BOARD_BONUSES[i];
    }
}

/*
 * Initialize full game state
 */
void game_init(GameState *game, uint32_t seed) {
    RNG rng;
    uint8_t i, p;

    /* Initialize RNG */
    rng_init(&rng, seed);

    /* Initialize board */
    board_init(game->board);

    /* Initialize and shuffle bag */
    bag_init(&game->bag);
    bag_shuffle(&game->bag, &rng);

    /* Initialize players */
    for (p = 0; p < NUM_PLAYERS; p++) {
        game->players[p].score = 0;
        game->players[p].rack.count = 0;
        for (i = 0; i < RACK_SIZE; i++) {
            game->players[p].rack.tiles[i] = TILE_EMPTY;
        }
        /* Draw initial tiles */
        rack_fill(&game->players[p].rack, &game->bag);
    }

    /* Game state */
    game->current_player = 0;
    game->game_over = 0;
    game->turn_count = 0;
}

/*
 * Convert tile value to display character
 */
char tile_to_char(uint8_t tile) {
    if (tile == TILE_EMPTY) {
        return ' ';
    } else if (tile >= TILE_A && tile <= TILE_Z) {
        return 'A' + (tile - TILE_A);
    } else if (tile == TILE_BLANK) {
        return '?';
    }
    return ' ';
}

/*
 * Convert character to tile value
 */
uint8_t char_to_tile(char c) {
    if (c >= 'A' && c <= 'Z') {
        return TILE_A + (c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        return TILE_A + (c - 'a');
    } else if (c == '?' || c == '*') {
        return TILE_BLANK;
    }
    return TILE_EMPTY;
}
