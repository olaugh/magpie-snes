/**
 * Prime Sieve ROM for sxtest
 *
 * Computes the first 100 prime numbers using the Sieve of Eratosthenes
 * and stores them in WRAM for verification by the test harness.
 *
 * Memory layout (WRAM at $7E0000, accessed via direct page mirrors):
 *   $7E0100 - $7E0357: Sieve array (600 bytes, 1 = composite)
 *   $7E0400 - $7E04C7: Prime results (100 x 16-bit words)
 *   $7E0600:           Prime count (16-bit word)
 *   $7E0602:           Done flag ($DEAD when complete)
 *
 * For cc65, we access low WRAM through direct addressing.
 */

/* cc65 type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/* Memory-mapped locations in WRAM (bank $7E, mirrored at $00:0000-$1FFF) */
#define SIEVE_ARRAY   ((volatile uint8_t*)0x0100)
#define PRIME_RESULTS ((volatile uint16_t*)0x0400)
#define PRIME_COUNT   ((volatile uint16_t*)0x0600)
#define DONE_FLAG     ((volatile uint16_t*)0x0602)

/* Constants */
#define SIEVE_SIZE    600   /* Check numbers 0-599 */
#define NUM_PRIMES    100   /* Find first 100 primes */
#define DONE_VALUE    0xDEAD

/**
 * Initialize the sieve array to all zeros (0 = potentially prime)
 */
static void clear_sieve(void)
{
    int i;
    for (i = 0; i < SIEVE_SIZE; i++) {
        SIEVE_ARRAY[i] = 0;
    }
}

/**
 * Mark 0 and 1 as composite (not prime)
 */
static void mark_trivial_composites(void)
{
    SIEVE_ARRAY[0] = 1;  /* 0 is not prime */
    SIEVE_ARRAY[1] = 1;  /* 1 is not prime */
}

/**
 * Run the Sieve of Eratosthenes algorithm
 * For each prime p, mark all multiples of p as composite
 */
static void run_sieve(void)
{
    int p;
    int multiple;

    /* Only need to check up to sqrt(SIEVE_SIZE) ≈ 24 */
    for (p = 2; p <= 24; p++) {
        /* Skip if already marked as composite */
        if (SIEVE_ARRAY[p]) {
            continue;
        }

        /* Mark all multiples of p as composite */
        for (multiple = p * 2; multiple < SIEVE_SIZE; multiple += p) {
            SIEVE_ARRAY[multiple] = 1;
        }
    }
}

/**
 * Collect the first NUM_PRIMES primes from the sieve into the results array
 */
static void collect_primes(void)
{
    int count = 0;
    int n;

    for (n = 2; n < SIEVE_SIZE && count < NUM_PRIMES; n++) {
        if (SIEVE_ARRAY[n] == 0) {
            /* n is prime */
            PRIME_RESULTS[count] = n;
            count++;
        }
    }

    /* Store the count */
    *PRIME_COUNT = count;
}

/**
 * Main entry point
 */
void main(void)
{
    /* Step 1: Clear the sieve array */
    clear_sieve();

    /* Step 2: Mark 0 and 1 as not prime */
    mark_trivial_composites();

    /* Step 3: Run the Sieve of Eratosthenes */
    run_sieve();

    /* Step 4: Collect primes into the results array */
    collect_primes();

    /* Step 5: Set the done flag to signal completion */
    *DONE_FLAG = DONE_VALUE;

    /* The startup code will loop forever after main returns */
}
