/*
 * File: day46-compilation.c
 * Goal: Master the C Compilation Process
 * Concepts: Preprocessing, compilation, assembly, linking, build systems, libraries
 * 
 * This file demonstrates the complete compilation pipeline!
 * 
 * Compile steps:
 *   1. Preprocess:  gcc -E day46-compilation.c > day46-compilation.i
 *   2. Compile:     gcc -S day46-compilation.i -o day46-compilation.s
 *   3. Assemble:    gcc -c day46-compilation.s -o day46-compilation.o
 *   4. Link:        gcc day46-compilation.o -o compilation
 * 
 * Or all at once:
 *   gcc -Wall -Wextra -Werror -std=c11 -O2 -g -o compilation day46-compilation.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ========== SECTION 1: PREPROCESSOR DEMONSTRATION ========== */

/* Macros for demonstration */
#define PROGRAM_NAME "C Compilation Demo"
#define PROGRAM_VERSION_MAJOR 1
#define PROGRAM_VERSION_MINOR 0
#define PROGRAM_VERSION_PATCH 0

/* Stringification macros */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/* Version string */
#define VERSION_STRING \
    TOSTRING(PROGRAM_VERSION_MAJOR) "." \
    TOSTRING(PROGRAM_VERSION_MINOR) "." \
    TOSTRING(PROGRAM_VERSION_PATCH)

/* Conditional compilation */
#ifdef DEBUG_MODE
    #define LOG_DEBUG(msg) printf("[DEBUG] %s\n", msg)
#else
    #define LOG_DEBUG(msg) /* Nothing */
#endif

#ifdef NDEBUG
    #define BUILD_TYPE "Release"
#else
    #define BUILD_TYPE "Debug"
#endif

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM "Windows"
#elif defined(__linux__)
    #define PLATFORM "Linux"
#elif defined(__APPLE__)
    #define PLATFORM "macOS"
#else
    #define PLATFORM "Unknown"
#endif

void demo_preprocessor(void) {
    puts("\n[1] Preprocessor Stage");
    
    printf("  Preprocessor output:\n");
    printf("    Program: %s\n", PROGRAM_NAME);
    printf("    Version: %s\n", VERSION_STRING);
    printf("    Build: %s\n", BUILD_TYPE);
    printf("    Platform: %s\n", PLATFORM);
    
    printf("\n  Preprocessor directives:\n");
    printf("    • #include - Insert header files\n");
    printf("    • #define - Define macros\n");
    printf("    • #if/#ifdef - Conditional compilation\n");
    printf("    • #error - Generate errors\n");
    printf("    • #pragma - Compiler directives\n\n");
    
    printf("  View preprocessor output:\n");
    printf("    gcc -E day46-compilation.c > output.i\n");
    printf("    # See all macros expanded!\n");
    
    LOG_DEBUG("Debug logging enabled");
}

/* ========== SECTION 2: COMPILATION STAGE ========== */

/* Function to demonstrate compilation */
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

/* Inline function */
static inline int square(int x) {
    return x * x;
}

/* Const function (optimization hint) */
int get_constant(void) {
    return 42;
}

void demo_compilation(void) {
    puts("\n[2] Compilation Stage");
    
    printf("  Compilation converts C to assembly:\n");
    printf("    • Parses C syntax\n");
    printf("    • Performs semantic analysis\n");
    printf("    • Generates intermediate representation\n");
    printf("    • Optimizes code\n");
    printf("    • Generates assembly code\n\n");
    
    printf("  Test functions:\n");
    printf("    add(10, 20) = %d\n", add(10, 20));
    printf("    multiply(5, 6) = %d\n", multiply(5, 6));
    printf("    square(7) = %d\n", square(7));
    printf("    get_constant() = %d\n", get_constant());
    
    printf("\n  Generate assembly output:\n");
    printf("    gcc -S day46-compilation.c -o output.s\n");
    printf("    # View assembly code!\n\n");
    
    printf("  Optimization levels:\n");
    printf("    -O0: No optimization (debug)\n");
    printf("    -O1: Basic optimization\n");
    printf("    -O2: Standard optimization (recommended)\n");
    printf("    -O3: Aggressive optimization\n");
    printf("    -Os: Optimize for size\n");
    printf("    -Ofast: Fast but may violate standards\n");
}

/* ========== SECTION 3: ASSEMBLY STAGE ========== */

/* Function to examine in assembly */
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

/* Iterative version */
int factorial_iterative(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

void demo_assembly(void) {
    puts("\n[3] Assembly Stage");
    
    printf("  Assembly converts assembly to machine code:\n");
    printf("    • Parses assembly instructions\n");
    printf("    • Generates object code (.o files)\n");
    printf("    • Creates symbol table\n");
    printf("    • Creates relocation information\n\n");
    
    printf("  Test functions:\n");
    printf("    factorial(5) = %d\n", factorial(5));
    printf("    factorial_iterative(5) = %d\n", factorial_iterative(5));
    
    printf("\n  Assemble to object file:\n");
    printf("    gcc -c day46-compilation.s -o output.o\n");
    printf("    # Creates relocatable object file\n\n");
    
    printf("  Inspect object file:\n");
    printf("    objdump -d output.o  # Disassemble\n");
    printf("    nm output.o          # List symbols\n");
    printf("    readelf -a output.o  # ELF information\n");
    printf("    size output.o        # Section sizes\n");
}

/* ========== SECTION 4: LINKING STAGE ========== */

/* External function declaration (will be linked) */
extern int printf(const char *format, ...);

/* Static function (not exported) */
static int internal_helper(int x) {
    return x * 2;
}

/* Function using internal helper */
int process_value(int x) {
    return internal_helper(x) + 10;
}

void demo_linking(void) {
    puts("\n[4] Linking Stage");
    
    printf("  Linking combines object files:\n");
    printf("    • Resolves external symbols\n");
    printf("    • Combines sections\n");
    printf("    • Performs relocations\n");
    printf("    • Creates executable\n\n");
    
    printf("  Test functions:\n");
    printf("    internal_helper(5) = %d (static, not exported)\n", 
           internal_helper(5));
    printf("    process_value(5) = %d\n", process_value(5));
    
    printf("\n  Link multiple object files:\n");
    printf("    gcc file1.o file2.o file3.o -o program\n\n");
    
    printf("  Link with libraries:\n");
    printf("    gcc program.o -lm -o program  # Math library\n");
    printf("    gcc program.o -lpthread -o program  # Threads\n");
    printf("    gcc program.o -L/path -lmylib -o program\n\n");
    
    printf("  Static vs. Dynamic linking:\n");
    printf("    Static (-static): Library code copied into executable\n");
    printf("    Dynamic (default): Library loaded at runtime\n");
}

/* ========== SECTION 5: LIBRARIES ========== */

/* Library function examples */
int lib_add(int a, int b) {
    return a + b;
}

int lib_subtract(int a, int b) {
    return a - b;
}

int lib_multiply(int a, int b) {
    return a * b;
}

int lib_divide(int a, int b) {
    if (b == 0) return -1;
    return a / b;
}

void demo_libraries(void) {
    puts("\n[5] Libraries");
    
    printf("  Static Library (.a):\n");
    printf("    • Archive of object files\n");
    printf("    • Code copied into executable\n");
    printf("    • Larger executable\n");
    printf("    • No runtime dependency\n");
    printf("    • Create: ar rcs libmylib.a *.o\n");
    printf("    • Link: gcc program.o -L. -lmylib -o program\n\n");
    
    printf("  Shared Library (.so/.dll):\n");
    printf("    • Dynamically loaded at runtime\n");
    printf("    • Smaller executable\n");
    printf("    • Runtime dependency\n");
    printf("    • Can update library without recompiling\n");
    printf("    • Create: gcc -shared -fPIC -o libmylib.so *.o\n");
    printf("    • Link: gcc program.o -L. -lmylib -o program\n\n");
    
    printf("  Test library functions:\n");
    printf("    lib_add(10, 5) = %d\n", lib_add(10, 5));
    printf("    lib_subtract(10, 5) = %d\n", lib_subtract(10, 5));
    printf("    lib_multiply(10, 5) = %d\n", lib_multiply(10, 5));
    printf("    lib_divide(10, 5) = %d\n", lib_divide(10, 5));
}

/* ========== SECTION 6: BUILD SYSTEMS ========== */

/* Build configuration */
typedef struct {
    const char *compiler;
    const char *cflags;
    const char *ldflags;
    const char *target;
} BuildConfig;

BuildConfig get_build_config(void) {
    BuildConfig config = {
        .compiler = "gcc",
        .cflags = "-Wall -Wextra -Werror -std=c11 -O2",
        .ldflags = "-lm",
        .target = "compilation"
    };
    return config;
}

void demo_makefile(void) {
    puts("\n[6] Build Systems - Makefile");
    
    printf("  Example Makefile:\n\n");
    printf("    CC = gcc\n");
    printf("    CFLAGS = -Wall -Wextra -Werror -std=c11 -O2\n");
    printf("    LDFLAGS = -lm\n");
    printf("    TARGET = compilation\n");
    printf("    SRCS = day46-compilation.c\n");
    printf("    OBJS = $(SRCS:.c=.o)\n\n");
    printf("    all: $(TARGET)\n\n");
    printf("    $(TARGET): $(OBJS)\n");
    printf("        $(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)\n\n");
    printf("    .c.o:\n");
    printf("        $(CC) $(CFLAGS) -c $< -o $@\n\n");
    printf("    clean:\n");
    printf("        rm -f $(OBJS) $(TARGET)\n\n");
    printf("    .PHONY: all clean\n\n");
    
    printf("  Common make commands:\n");
    printf("    make          # Build all\n");
    printf("    make clean    # Remove build artifacts\n");
    printf("    make -j4      # Parallel build (4 jobs)\n");
}

void demo_cmake(void) {
    puts("\n[7] Build Systems - CMake");
    
    printf("  Example CMakeLists.txt:\n\n");
    printf("    cmake_minimum_required(VERSION 3.10)\n");
    printf("    project(CompilationDemo C)\n\n");
    printf("    set(CMAKE_C_STANDARD 11)\n");
    printf("    set(CMAKE_C_STANDARD_REQUIRED ON)\n\n");
    printf("    add_compile_options(-Wall -Wextra -Werror)\n\n");
    printf("    add_executable(compilation day46-compilation.c)\n\n");
    printf("    target_link_libraries(compilation m)\n\n");
    printf("    # Build commands:\n");
    printf("    mkdir build && cd build\n");
    printf("    cmake ..\n");
    printf("    make\n\n");
    
    printf("  CMake benefits:\n");
    printf("    • Cross-platform (Windows, Linux, macOS)\n");
    printf("    • Out-of-source builds\n");
    printf("    • Dependency management\n");
    printf("    • IDE integration\n");
    printf("    • Large ecosystem\n");
}

/* ========== SECTION 8: DEBUG SYMBOLS ========== */

/* Function for debugging demonstration */
int debug_function(int x, int y) {
    int sum = x + y;
    int product = x * y;
    int result = sum + product;
    return result;
}

void demo_debug_symbols(void) {
    puts("\n[8] Debug Symbols");
    
    printf("  Debug symbols (-g flag):\n");
    printf("    • Include source file names\n");
    printf("    • Include line numbers\n");
    printf("    • Include variable names\n");
    printf("    • Include function names\n");
    printf("    • Enable debugger (gdb)\n\n");
    
    printf("  Compile with debug symbols:\n");
    printf("    gcc -g -O0 -o program program.c\n");
    printf("    # -g: Add debug symbols\n");
    printf("    # -O0: No optimization (easier debugging)\n\n");
    
    printf("  Debug with gdb:\n");
    printf("    gdb ./program\n");
    printf("    (gdb) break main\n");
    printf("    (gdb) run\n");
    printf("    (gdb) next\n");
    printf("    (gdb) print variable\n");
    printf("    (gdb) backtrace\n\n");
    
    printf("  Test debug function:\n");
    printf("    debug_function(5, 3) = %d\n", debug_function(5, 3));
    
    printf("\n  Strip debug symbols (for release):\n");
    printf("    strip program  # Remove debug symbols\n");
    printf("    # Reduces executable size\n");
}

/* ========== SECTION 9: OPTIMIZATION ========== */

/* Function to demonstrate optimization */
int optimize_me(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i;
    }
    return sum;
}

/* Const function for optimization */
int get_value(void) {
    return 100;
}

void demo_optimization(void) {
    puts("\n[9] Optimization");
    
    printf("  Optimization levels:\n");
    printf("    -O0: No optimization (fastest compile, debug)\n");
    printf("    -O1: Basic optimization\n");
    printf("    -O2: Standard optimization (recommended)\n");
    printf("    -O3: Aggressive optimization (may increase size)\n");
    printf("    -Os: Optimize for size\n");
    printf("    -Ofast: Fast but may violate IEEE/standards\n\n");
    
    printf("  Common optimization flags:\n");
    printf("    -funroll-loops      # Unroll loops\n");
    printf("    -finline-functions  # Inline small functions\n");
    printf("    -ffast-math         # Fast floating-point (non-standard)\n");
    printf("    -fomit-frame-pointer # Save register\n");
    printf("    -march=native       # Optimize for current CPU\n\n");
    
    printf("  Test optimization:\n");
    printf("    optimize_me(100) = %d\n", optimize_me(100));
    
    printf("\n  Compare optimization levels:\n");
    printf("    gcc -O0 -S program.c -o program_O0.s\n");
    printf("    gcc -O2 -S program.c -o program_O2.s\n");
    printf("    diff program_O0.s program_O2.s\n");
    printf("    # See optimization differences!\n");
}

/* ========== SECTION 10: COMMON ERRORS ========== */

void demo_common_errors(void) {
    puts("\n[10] Common Compilation Errors");
    
    printf("  Preprocessor errors:\n");
    printf("    • #include <file.h>: File not found\n");
    printf("    • #error: Intentional error\n");
    printf("    • Macro redefinition warnings\n\n");
    
    printf("  Compilation errors:\n");
    printf("    • Syntax errors\n");
    printf("    • Type mismatches\n");
    printf("    • Undeclared variables\n");
    printf("    • Missing semicolons\n\n");
    
    printf("  Linker errors:\n");
    printf("    • undefined reference to 'function'\n");
    printf("    • multiple definition of 'function'\n");
    printf("    • cannot find -llibrary\n\n");
    
    printf("  Debugging tips:\n");
    printf("    1. Read error message carefully\n");
    printf("    2. Check line number\n");
    printf("    3. Search for similar errors online\n");
    printf("    4. Use -v for verbose output\n");
    printf("    5. Use -E to see preprocessor output\n");
    printf("    6. Use -S to see assembly output\n");
}

/* ========== SECTION 11: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[11] Compilation Best Practices");
    
    printf("  1. Always use warnings:\n");
    printf("     -Wall -Wextra -Wpedantic -Werror\n\n");
    
    printf("  2. Specify C standard:\n");
    printf("     -std=c11 or -std=c17\n\n");
    
    printf("  3. Use optimization for release:\n");
    printf("     -O2 for most cases\n\n");
    
    printf("  4. Use debug symbols for development:\n");
    printf("     -g -O0 for debugging\n\n");
    
    printf("  5. Use build systems:\n");
    printf("     Make for simple projects\n");
    printf("     CMake for complex/cross-platform\n\n");
    
    printf("  6. Separate build directory:\n");
    printf("     mkdir build && cd build\n");
    printf("     cmake .. && make\n\n");
    
    printf("  7. Version your binaries:\n");
    printf("     Embed version in executable\n\n");
    
    printf("  8. Test multiple optimization levels:\n");
    printf("     Ensure code works at -O0, -O2, -O3\n\n");
    
    printf("  9. Document build requirements:\n");
    printf("     Compiler version, flags, dependencies\n\n");
    
    printf("  10. Use CI/CD for builds:\n");
    printf("      Automate compilation and testing\n");
}

/* ========== SECTION 12: COMPLETE BUILD EXAMPLE ========== */

void demo_complete_build(void) {
    puts("\n[12] Complete Build Example");
    
    printf("  Step-by-step compilation:\n\n");
    
    printf("  1. Preprocess:\n");
    printf("     gcc -E day46-compilation.c > day46-compilation.i\n");
    printf("     # Expands all macros and includes\n\n");
    
    printf("  2. Compile:\n");
    printf("     gcc -S day46-compilation.i -o day46-compilation.s\n");
    printf("     # Generates assembly code\n\n");
    
    printf("  3. Assemble:\n");
    printf("     gcc -c day46-compilation.s -o day46-compilation.o\n");
    printf("     # Generates object file\n\n");
    
    printf("  4. Link:\n");
    printf("     gcc day46-compilation.o -o compilation\n");
    printf("     # Creates executable\n\n");
    
    printf("  Or all at once:\n");
    printf("     gcc -Wall -Wextra -Werror -std=c11 -O2 -g \\\n");
    printf("         -o compilation day46-compilation.c\n\n");
    
    printf("  Inspect the executable:\n");
    printf("     file compilation        # File type\n");
    printf("     size compilation        # Section sizes\n");
    printf("     ldd compilation         # Dynamic libraries\n");
    printf("     nm compilation          # Symbols\n");
    printf("     objdump -d compilation  # Disassemble\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 46: C Compilation Process ===");
    puts("From Source to Executable: Preprocessing, Compilation, Assembly, Linking\n");
    
    /* Section 1: Preprocessor */
    demo_preprocessor();
    
    /* Section 2: Compilation */
    demo_compilation();
    
    /* Section 3: Assembly */
    demo_assembly();
    
    /* Section 4: Linking */
    demo_linking();
    
    /* Section 5: Libraries */
    demo_libraries();
    
    /* Section 6: Makefile */
    demo_makefile();
    
    /* Section 7: CMake */
    demo_cmake();
    
    /* Section 8: Debug symbols */
    demo_debug_symbols();
    
    /* Section 9: Optimization */
    demo_optimization();
    
    /* Section 10: Common errors */
    demo_common_errors();
    
    /* Section 11: Best practices */
    demo_best_practices();
    
    /* Section 12: Complete build */
    demo_complete_build();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 46: C Compilation Process complete!");
    puts("Key takeaways:");
    puts("  • Four stages: Preprocess → Compile → Assemble → Link");
    puts("  • Use -E to view preprocessor output");
    puts("  • Use -S to generate assembly code");
    puts("  • Use -c to create object files");
    puts("  • Use -g for debug symbols");
    puts("  • Use -O2 for release optimization");
    puts("  • Use -Wall -Wextra -Werror for warnings");
    puts("  • Static libraries (.a) vs. shared libraries (.so/.dll)");
    puts("  • Use Make or CMake for build automation");
    puts("  • Understand linker errors (undefined reference)");
    puts("\n🎓 You're mastering the C compilation pipeline!");
    puts("   Understanding compilation helps debug and optimize! 🚀\n");
    
    return EXIT_SUCCESS;
}