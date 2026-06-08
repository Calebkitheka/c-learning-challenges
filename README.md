# 🎯 C Learning Journey

> A structured, project-based path to mastering C programming — from basics to advanced systems programming.

![C Programming](https://img.shields.io/badge/C-ANSI%2FC99%2FC11-blue)
![Progress](https://img.shields.io/badge/Progress-40%2Finfinity%20Days-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## 📋 Overview

This repository documents my self-taught journey learning C programming through daily challenges, hands-on projects, and progressive skill-building. Each day focuses on specific concepts with working code examples.

**Learning Philosophy:**
- ✅ Understand the concept first, then code your own version
- ✅ Build small, testable programs daily
- ✅ Commit progress to GitHub for accountability
- ✅ Focus on safety, portability, and best practices

---

## 🗓️ Learning Path Progress

### 🔹 Foundation (Days 1-5)

| Day | Topic | Key Concepts | Files |
|-----|-------|-------------|-------|
| 1 | Hello World & Basic I/O | `printf`, `scanf`, compilation, `gcc` flags | `day01-hello.c` |
| 2 | Variables & Data Types | `int`, `float`, `char`, `sizeof`, type casting | `day02-variables.c` |
| 3 | Operators & Expressions | Arithmetic, relational, logical, bitwise operators | `day03-operators.c` |
| 4 | Control Flow: Conditionals | `if`, `else`, `switch`, ternary operator | `day04-conditionals.c` |
| 5 | String Utilities Lab | `strcpy`, `strcat`, `strcmp`, safe string handling | `day05-string-toolkit.c` |

### 🔹 Core Concepts (Days 6-10)

| Day | Topic | Key Concepts | Files |
|-----|-------|-------------|-------|
| 6 | Literals & Encodings | Floating suffixes, string encodings, escape sequences | `day06-literal-explorer.c` |
| 7 | Compound Literals | Unnamed objects, designated initializers, scope rules | `day07-compound-literals.c` |
| 8 | Bit-fields & Packing | Bit-field syntax, unions, hardware register patterns | `day08-bitfield-toolkit.c` |
| 9 | Arrays Deep Dive | Initialization, decay, row-major order, multidimensional | `day09-array-toolkit.c` |
| 9b | Array Mechanics | Access patterns, zeroing, dynamic allocation, bounds checking | `day09b-array-mechanics.c` |

### 🔹 Data Structures (Days 10-14)

| Day | Topic | Key Concepts | Files |
|-----|-------|-------------|-------|
| 10 | Linked Lists | Node creation, insertion, reversal, memory management | `day10-linked-list-toolkit.c` |
| 11 | Enumerations | Enum syntax, designated initializers, typedef patterns | `day11-enum-toolkit.c` |
| 12 | Structs & Initialization | Struct syntax, designated initializers, opaque pointers | `day12-struct-toolkit.c` |
| 13 | Standard Math Library | `pow`, `fmod`, error handling, precision types | `day13-math-toolkit.c` |
| 14 | Iteration Statements | `for`, `while`, `do-while`, loop unrolling, Duff's Device | `day14-loop-toolkit.c` |

### 🔹 Modular Programming (Days 15-20)

| Day | Topic | Key Concepts | Files |
|-----|-------|-------------|-------|
| 15 | Selection Statements | `if`/`else`, `switch`, fall-through, enum handling | `day15-selection-toolkit.c` |
| 16 | Initialization Rules | Storage duration, designated initializers, one-definition rule | `day16-initialization-toolkit.c` |
| 17 | Declaration vs Definition | Header/source separation, linkage, opaque pointers | `decl_def_demo/` |
| 18 | Command-Line Arguments | `argc`/`argv`, `strtol`, `getopt`, argument parsing | `day18-cli-toolkit.c` |
| 19 | File I/O Basics | `fopen`, `fprintf`, `getline`, binary vs text modes | `day19-file-io-toolkit.c` |
| 19b | File Reading Patterns | `fscanf`, `fgets`, portable binary serialization | `day19b-file-reading-toolkit.c` |
| 20 | Formatted Output | Conversion specifiers, flags, precision, safety | `day20-format-toolkit.c` |
| 20b | Portable Printing | Pointer printing, length modifiers, cross-platform | `day20b-portable-printing.c` |

### 🔹 Memory & Pointers (Days 21-23)

| Day | Topic | Key Concepts | Files |
|-----|-------|-------------|-------|
| 21 | Struct Pointers & Const | Struct pointers, const correctness, function pointers | `day21-pointer-toolkit.c` |
| 22 | Dynamic Memory | `malloc`, `calloc`, `realloc`, `free`, leak prevention | `day22-memory-toolkit.c` |
| 23 | Advanced Pointer Patterns | Pointer arithmetic, double pointers, polymorphism | `day23-advanced-pointers.c` |
| 24 | Error Handling | `errno`, `perror`, custom error codes, assertions |'day24-error-handling.c' |
| 25 | Preprocessor Deep Dive | Macros, conditionals, include guards, X-macros | `day25-preprocessor.c` |

### 🔹 Upcoming (Days 24-30)

| Day | Planned Topic | Goals |
|-----|--------------|-------|
| 26 | Standard Library Tour | `<string.h>`, `<ctype.h>`, `<time.h>`, `<stdlib.h>` | `day27-mini-shell.c` |
| 27 | Project: Mini Shell | Command parsing, execution, pipes, basic REPL |  `day28-config-parser.c` |
| 28 | Project: Config Parser | INI/JSON parsing, validation, defaults merging |  `day29-serializer.c` |
| 29 | Project: Data Serializer | Binary/text serialization, versioning, checksums |
| 30 | Capstone Review | Refactor early code, apply all learned patterns |

### 🔹 Advanced Topics (Day 31+)

| Day | Topic | Key Concepts | Files |
|-----|-------|-------------|-------|
| 31 | Sequence Points | Evaluation order, undefined behavior | `day31-sequence-points.c` |
| 32 | Function Pointers Deep Dive | Callbacks, vtables, command tables | `day32-function-pointers.c` |
| 33a | Function Parameters | Pass mechanisms, const, variadic | `day33-function-parameters.c` |
| 33b | 2D Arrays to Functions | 5 methods, memory layout, VLA | `day33-2d-arrays.c` |
| 34 | Multithreading | pthreads, mutexes, condition variables | `day34-multithreading.c` |
| 35 | Advanced Error Handling | Propagation, logging, recovery, thread-safe | `day35-advanced-error-handling.c` |
| 36 | Undefined Behavior | UB patterns, sanitizers, prevention | `day36-undefined-behavior.c` |
| 37 | Random Number Generation | rand(), PCG, crypto RNG, testing | `day37-random-numbers.c` |
| 38 | Advanced Preprocessor | Advanced macros, computations, debugging | `day38-advanced-macros.c` |
| 39 | Signal Handling | Signals, handlers, sigaction, safety, masks | `day39-signal-handling.c` |
| 40 | **Variable Arguments** | va_list, format strings, type safety, forwarding | `day40-variadic-functions.c` |
| ... | Continue Learning! | Network, embedded, system programming | Future projects |

## 🛠️ Build & Run Instructions

### Prerequisites
- GCC compiler (MinGW-w64 for Windows, or system GCC for Linux/macOS)
- Git for version control
- VS Code (recommended) or any text editor

### Compile Any Day's Program
```bash
# Navigate to project root
cd c-learning-challenges

# Compile a specific day (example: Day 23)
gcc -Wall -Wextra -Werror -std=c11 -o day23_advanced day23-advanced-pointers.c

# Run
./day23_advanced          # Linux/macOS
day23_advanced.exe        # Windows
