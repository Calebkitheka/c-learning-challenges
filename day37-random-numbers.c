/*
 * File: day37-random-numbers.c
 * Goal: Master random number generation in C
 * Concepts: rand(), seeding, ranges, quality, modern RNG, cryptographic RNG
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o random_numbers day37-random-numbers.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
    #include <wincrypt.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
#endif

/* ========== SECTION 1: BASIC RAND() USAGE ========== */

void demo_basic_rand(void) {
    puts("\n[1] Basic rand() and srand()");
    
    printf("  RAND_MAX = %d\n", RAND_MAX);
    printf("  Note: RAND_MAX varies by platform (minimum 32767)\n\n");
    
    /* Without seeding - will produce same sequence every run */
    printf("  Without srand() (same sequence every run):\n");
    for (int i = 0; i < 5; i++) {
        printf("    rand() = %d\n", rand());
    }
    
    /* With seeding */
    printf("\n  With srand(time(NULL)) (different each run):\n");
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 5; i++) {
        printf("    rand() = %d\n", rand());
    }
    
    /* Fixed seed for reproducibility */
    printf("\n  With fixed seed (reproducible for testing):\n");
    srand(42);
    for (int i = 0; i < 5; i++) {
        printf("    rand() = %d\n", rand());
    }
    
    printf("\n  ⚠️  rand() limitations:\n");
    printf("     • Low quality (not suitable for simulations)\n");
    printf("     • Not thread-safe\n");
    printf("     • Not cryptographically secure\n");
    printf("     • Implementation varies by platform\n");
}

/* ========== SECTION 2: GENERATING RANGES ========== */

/* Generate random int in [0, max] */
int rand_range(int max) {
    return rand() % (max + 1);
}

/* Generate random int in [min, max] */
int rand_range_min_max(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    return min + rand() % (max - min + 1);
}

/* Generate random float in [0.0, 1.0] */
double rand_float(void) {
    return (double)rand() / RAND_MAX;
}

/* Generate random float in [min, max] */
double rand_float_range(double min, double max) {
    return min + rand_float() * (max - min);
}

/* Generate random boolean */
bool rand_bool(void) {
    return rand() % 2 == 0;
}

/* Better range generation (reduces bias) */
int rand_range_better(int max) {
    /* Use higher bits which have better randomness */
    return (int)((double)rand() / (RAND_MAX + 1.0) * (max + 1));
}

void demo_ranges(void) {
    puts("\n[2] Generating Random Ranges");
    
    srand((unsigned int)time(NULL));
    
    printf("  Random int [0, 9]: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", rand_range(9));
    }
    printf("\n");
    
    printf("  Random int [1, 100]: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", rand_range_min_max(1, 100));
    }
    printf("\n");
    
    printf("  Random int [-5, 5]: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", rand_range_min_max(-5, 5));
    }
    printf("\n");
    
    printf("  Random float [0.0, 1.0]: ");
    for (int i = 0; i < 5; i++) {
        printf("%.3f ", rand_float());
    }
    printf("\n");
    
    printf("  Random float [-1.0, 1.0]: ");
    for (int i = 0; i < 5; i++) {
        printf("%.3f ", rand_float_range(-1.0, 1.0));
    }
    printf("\n");
    
    printf("  Random boolean: ");
    for (int i = 0; i < 10; i++) {
        printf("%s ", rand_bool() ? "true" : "false");
    }
    printf("\n");
}

/* ========== SECTION 3: BETTER RNG ALGORITHMS ========== */

/* xorshift32 - Simple, fast, good quality */
typedef struct {
    uint32_t state;
} XorShift32;

void xorshift32_init(XorShift32 *rng, uint32_t seed) {
    rng->state = seed ? seed : 1;  /* Can't be zero */
}

uint32_t xorshift32_next(XorShift32 *rng) {
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

/* PCG32 - Better quality, still fast */
typedef struct {
    uint64_t state;
    uint64_t inc;
} PCG32;

void pcg32_init(PCG32 *rng, uint64_t seed, uint64_t seq) {
    rng->state = 0;
    rng->inc = (seq << 1) | 1;  /* Must be odd */
    pcg32_next(rng);  /* Advance once */
    rng->state += seed;
    pcg32_next(rng);  /* Advance again */
}

uint32_t pcg32_next(PCG32 *rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18) ^ oldstate) >> 27);
    uint32_t rot = (uint32_t)(oldstate >> 59);
    
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

/* Generate random number in range using PCG */
uint32_t pcg32_range(PCG32 *rng, uint32_t max) {
    return pcg32_next(rng) % (max + 1);
}

void demo_modern_rng(void) {
    puts("\n[3] Modern RNG Algorithms");
    
    /* xorshift32 */
    printf("  xorshift32:\n");
    XorShift32 xs;
    xorshift32_init(&xs, (uint32_t)time(NULL));
    printf("    ");
    for (int i = 0; i < 10; i++) {
        printf("%u ", xorshift32_next(&xs));
    }
    printf("\n");
    printf("    • Fast, good quality, small state\n");
    printf("    • Period: 2^32 - 1\n");
    
    /* PCG32 */
    printf("\n  PCG32 (Permuted Congruential Generator):\n");
    PCG32 pcg;
    pcg32_init(&pcg, (uint64_t)time(NULL), 1);
    printf("    ");
    for (int i = 0; i < 10; i++) {
        printf("%u ", pcg32_next(&pcg));
    }
    printf("\n");
    printf("    • Excellent quality, fast, small state\n");
    printf("    • Period: 2^64\n");
    printf("    • Better statistical properties than rand()\n");
    
    printf("\n  Comparison:\n");
    printf("    rand():      Simple, poor quality, not portable\n");
    printf("    xorshift32:  Fast, good quality, easy to implement\n");
    printf("    PCG32:       Fast, excellent quality, recommended\n");
    printf("    Mersenne:    Slower, excellent quality, large state\n");
}

/* ========== SECTION 4: CRYPTOGRAPHIC RNG ========== */

/* Generate cryptographically secure random bytes */
bool crypto_random_bytes(void *buffer, size_t length) {
    if (!buffer || length == 0) return false;
    
    #ifdef _WIN32
        /* Windows: Use CryptGenRandom */
        HCRYPTPROV hProv;
        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 
                                 CRYPT_VERIFYCONTEXT)) {
            return false;
        }
        
        bool success = CryptGenRandom(hProv, (DWORD)length, (BYTE*)buffer);
        CryptReleaseContext(hProv, 0);
        return success;
    #else
        /* Unix/Linux: Use /dev/urandom */
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0) {
            return false;
        }
        
        ssize_t bytes_read = read(fd, buffer, length);
        close(fd);
        
        return bytes_read == (ssize_t)length;
    #endif
}

/* Generate cryptographically secure random int */
uint32_t crypto_random_int(void) {
    uint32_t value;
    if (crypto_random_bytes(&value, sizeof(value))) {
        return value;
    }
    /* Fallback to rand() if crypto RNG fails */
    return (uint32_t)rand();
}

/* Generate random password */
void generate_password(char *buffer, size_t length) {
    const char charset[] = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()_+-=[]{}|;:,.<>?";
    
    size_t charset_len = sizeof(charset) - 1;
    
    for (size_t i = 0; i < length - 1; i++) {
        uint32_t rand_val = crypto_random_int();
        buffer[i] = charset[rand_val % charset_len];
    }
    buffer[length - 1] = '\0';
}

void demo_crypto_rng(void) {
    puts("\n[4] Cryptographic Random Number Generation");
    
    printf("  Cryptographically secure random bytes:\n");
    uint8_t bytes[16];
    if (crypto_random_bytes(bytes, sizeof(bytes))) {
        printf("    ");
        for (size_t i = 0; i < sizeof(bytes); i++) {
            printf("%02X ", bytes[i]);
        }
        printf("\n");
        printf("    ✓ Suitable for:\n");
        printf("       • Passwords and tokens\n");
        printf("       • Encryption keys\n");
        printf("       • Session IDs\n");
        printf("       • Security-sensitive operations\n");
    } else {
        printf("    ⚠️  Crypto RNG not available (using fallback)\n");
    }
    
    printf("\n  Random password generation:\n");
    char password[33];
    generate_password(password, sizeof(password));
    printf("    Generated: %s\n", password);
    printf("    ✓ 32 characters, mixed charset\n");
    
    printf("\n  ⚠️  When to use cryptographic RNG:\n");
    printf("     ✓ Security-sensitive applications\n");
    printf("     ✓ Password/token generation\n");
    printf("     ✓ Encryption keys\n");
    printf("     ✗ Games and simulations (use PCG instead)\n");
    printf("     ✗ Performance-critical code (crypto is slower)\n");
}

/* ========== SECTION 5: STATISTICAL TESTING ========== */

/* Chi-squared test for uniformity */
void test_uniformity(void) {
    puts("\n[5] Testing RNG Quality");
    
    const int bins = 10;
    const int samples = 10000;
    int histogram[bins] = {0};
    
    /* Generate samples */
    for (int i = 0; i < samples; i++) {
        int bin = rand() % bins;
        histogram[bin]++;
    }
    
    /* Calculate expected count per bin */
    double expected = (double)samples / bins;
    
    /* Calculate chi-squared statistic */
    double chi_squared = 0.0;
    for (int i = 0; i < bins; i++) {
        double diff = histogram[i] - expected;
        chi_squared += (diff * diff) / expected;
    }
    
    printf("  Chi-squared test for uniformity:\n");
    printf("    Samples: %d\n", samples);
    printf("    Bins: %d\n", bins);
    printf("    Expected per bin: %.0f\n", expected);
    printf("    Chi-squared statistic: %.2f\n", chi_squared);
    printf("    Degrees of freedom: %d\n", bins - 1);
    
    /* Critical value for df=9, p=0.05 is approximately 16.92 */
    if (chi_squared < 16.92) {
        printf("    ✓ PASS: Distribution appears uniform\n");
    } else {
        printf("    ⚠️  WARNING: Distribution may not be uniform\n");
    }
    
    /* Print histogram */
    printf("\n    Histogram:\n");
    int max_count = 0;
    for (int i = 0; i < bins; i++) {
        if (histogram[i] > max_count) {
            max_count = histogram[i];
        }
    }
    
    for (int i = 0; i < bins; i++) {
        printf("      %d: ", i);
        int bar_len = (int)(50.0 * histogram[i] / max_count);
        for (int j = 0; j < bar_len; j++) {
            printf("█");
        }
        printf(" (%d)\n", histogram[i]);
    }
}

/* ========== SECTION 6: THREAD-SAFE RNG ========== */

/* Thread-local RNG state (C11) */
#ifdef __STDC_NO_THREADS__
    /* No thread support */
    typedef struct {
        PCG32 rng;
    } ThreadRNG;
#else
    #include <threads.h>
    thread_local PCG32 thread_rng;
    thread_local bool thread_rng_initialized = false;
#endif

/* Initialize thread-local RNG */
void init_thread_rng(void) {
    #ifndef __STDC_NO_THREADS__
    if (!thread_rng_initialized) {
        /* Use thread ID as part of seed for uniqueness */
        uint64_t seed = (uint64_t)time(NULL) ^ (uint64_t)(uintptr_t)thrd_current();
        pcg32_init(&thread_rng, seed, (uint64_t)(uintptr_t)thrd_current());
        thread_rng_initialized = true;
    }
    #endif
}

/* Thread-safe random number */
uint32_t thread_random(void) {
    #ifndef __STDC_NO_THREADS__
    init_thread_rng();
    return pcg32_next(&thread_rng);
    #else
    /* Fallback for non-threaded builds */
    static PCG32 rng;
    static bool initialized = false;
    if (!initialized) {
        pcg32_init(&rng, (uint64_t)time(NULL), 1);
        initialized = true;
    }
    return pcg32_next(&rng);
    #endif
}

void demo_thread_safe_rng(void) {
    puts("\n[6] Thread-Safe Random Number Generation");
    
    printf("  Thread-safe RNG using thread-local storage:\n");
    printf("    • Each thread has its own RNG state\n");
    printf("    • No locking required (fast)\n");
    printf("    • Different sequences per thread\n");
    
    printf("\n  Sample values:\n");
    printf("    ");
    for (int i = 0; i < 10; i++) {
        printf("%u ", thread_random());
    }
    printf("\n");
    
    printf("\n  ⚠️  rand() is NOT thread-safe:\n");
    printf("     • Uses global state\n");
    printf("     • Race conditions in multi-threaded code\n");
    printf("     • Use thread-local or per-instance RNG instead\n");
}

/* ========== SECTION 7: PRACTICAL APPLICATIONS ========== */

/* Shuffle array (Fisher-Yates) */
void shuffle_array(int *arr, size_t n) {
    PCG32 rng;
    pcg32_init(&rng, (uint64_t)time(NULL), 1);
    
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = pcg32_range(&rng, (uint32_t)i);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

/* Weighted random selection */
int weighted_random(const int *weights, size_t n) {
    /* Calculate total weight */
    int total = 0;
    for (size_t i = 0; i < n; i++) {
        total += weights[i];
    }
    
    if (total == 0) return -1;
    
    /* Generate random number in [0, total) */
    PCG32 rng;
    pcg32_init(&rng, (uint64_t)time(NULL), 1);
    int random = pcg32_range(&rng, (uint32_t)(total - 1));
    
    /* Find which weight bucket */
    int cumulative = 0;
    for (size_t i = 0; i < n; i++) {
        cumulative += weights[i];
        if (random < cumulative) {
            return (int)i;
        }
    }
    
    return (int)(n - 1);  /* Fallback */
}

/* Roll dice */
int roll_dice(int sides, int count) {
    PCG32 rng;
    pcg32_init(&rng, (uint64_t)time(NULL), 1);
    
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += (int)pcg32_range(&rng, (uint32_t)(sides - 1)) + 1;
    }
    return total;
}

void demo_applications(void) {
    puts("\n[7] Practical RNG Applications");
    
    /* Shuffle */
    printf("  Fisher-Yates shuffle:\n");
    int cards[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    printf("    Before: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", cards[i]);
    }
    shuffle_array(cards, 10);
    printf("\n    After:  ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", cards[i]);
    }
    printf("\n");
    
    /* Weighted random */
    printf("\n  Weighted random selection:\n");
    int weights[] = {10, 20, 30, 40};  /* Higher weight = more likely */
    int histogram[4] = {0};
    
    for (int i = 0; i < 1000; i++) {
        int selection = weighted_random(weights, 4);
        if (selection >= 0 && selection < 4) {
            histogram[selection]++;
        }
    }
    
    printf("    Weights: [10, 20, 30, 40]\n");
    printf("    Results (1000 trials):\n");
    for (int i = 0; i < 4; i++) {
        printf("      Index %d: %d times (%.1f%%)\n", 
               i, histogram[i], 100.0 * histogram[i] / 1000);
    }
    
    /* Dice rolling */
    printf("\n  Dice rolling:\n");
    printf("    Roll 2d6 (10 times): ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", roll_dice(6, 2));
    }
    printf("\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] RNG Best Practices");
    
    printf("  1. Seed once at program start:\n");
    printf("     srand(time(NULL));  // Not in a loop!\n\n");
    
    printf("  2. Use better RNG than rand():\n");
    printf("     PCG32 or xorshift for games/simulations\n");
    printf("     Crypto RNG for security-sensitive code\n\n");
    
    printf("  3. Avoid modulo bias:\n");
    printf("     Use: (int)((double)rand() / (RAND_MAX + 1.0) * n)\n");
    printf("     Or use RNG with built-in range function\n\n");
    
    printf("  4. Use thread-local RNG for multi-threading:\n");
    printf("     Don't share RNG state between threads\n\n");
    
    printf("  5. Save seed for reproducibility:\n");
    printf("     Store seed to reproduce test results\n\n");
    
    printf("  6. Test RNG quality:\n");
    printf("     Use statistical tests (chi-squared, etc.)\n\n");
    
    printf("  7. Choose RNG based on use case:\n");
    printf("     Games: PCG32, xorshift\n");
    printf("     Simulations: Mersenne Twister, PCG\n");
    printf("     Security: /dev/urandom, CryptGenRandom\n");
    printf("     Testing: Fixed seed for reproducibility\n\n");
    
    printf("  8. Never use rand() for:\n");
    printf("     ✗ Cryptography\n");
    printf("     ✗ Gambling/gaming (real money)\n");
    printf("     ✗ Security tokens\n");
    printf("     ✗ High-quality simulations\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 37: Random Number Generation ===");
    puts("From rand() to modern RNG: quality, security, best practices\n");
    
    /* Section 1: Basic rand() */
    demo_basic_rand();
    
    /* Section 2: Ranges */
    demo_ranges();
    
    /* Section 3: Modern RNG */
    demo_modern_rng();
    
    /* Section 4: Crypto RNG */
    demo_crypto_rng();
    
    /* Section 5: Statistical testing */
    test_uniformity();
    
    /* Section 6: Thread-safe */
    demo_thread_safe_rng();
    
    /* Section 7: Applications */
    demo_applications();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 37: Random Number Generation complete!");
    puts("Key takeaways:");
    puts("  • rand() is simple but low quality - avoid for serious work");
    puts("  • Always seed with srand() before using rand()");
    puts("  • Use PCG32 or xorshift for games/simulations");
    puts("  • Use cryptographic RNG for security-sensitive code");
    puts("  • Avoid modulo bias in range generation");
    puts("  • Use thread-local RNG for multi-threaded code");
    puts("  • Test RNG quality with statistical tests");
    puts("  • Save seeds for reproducible testing");
    puts("  • Choose RNG based on use case (speed vs. quality vs. security)");
    puts("\n🎓 You're generating quality random numbers!");
    puts("   Good RNG is essential for games, simulations, and security! 🚀\n");
    
    return EXIT_SUCCESS;
}