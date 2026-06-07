/*
 * File: day39-signal-handling.c
 * Goal: Master signal handling in C (Systems Programming)
 * Concepts: Signal handlers, sigaction, signal safety, masks, real-world patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o signal_handling day39-signal-handling.c
 * 
 * ⚠️  WARNING: Signal handling is advanced! Test carefully.
 *              Some demonstrations require user interaction (Ctrl+C).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ========== SECTION 1: SIGNAL HANDLER BASICS ========== */

/* Flag to communicate from signal handler to main code */
static volatile sig_atomic_t got_sigint = 0;
static volatile sig_atomic_t got_sigterm = 0;
static volatile sig_atomic_t got_sighup = 0;

/* Simple signal handler */
void sigint_handler(int sig) {
    /* Only async-signal-safe functions here! */
    got_sigint = 1;  /* Set flag */
    
    /* Safe to write to stderr in handler */
    const char msg[] = "\n  [SIGNAL] Caught SIGINT (Ctrl+C)\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

/* SIGTERM handler for graceful shutdown */
void sigterm_handler(int sig) {
    got_sigterm = 1;
    
    const char msg[] = "\n  [SIGNAL] Caught SIGTERM (shutdown request)\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

/* SIGHUP handler for config reload */
void sighup_handler(int sig) {
    got_sighup = 1;
    
    const char msg[] = "\n  [SIGNAL] Caught SIGHUP (reload config)\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

/* SIGSEGV handler for crash reporting */
void sigsegv_handler(int sig) {
    const char msg[] = "\n  [SIGNAL] Caught SIGSEGV (segmentation fault!)\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    
    /* Exit immediately - can't recover from segfault */
    _exit(1);  /* _exit is async-signal-safe, exit is not */
}

void demo_basic_handlers(void) {
    puts("\n[1] Basic Signal Handlers");
    
    printf("  Registering signal handlers:\n");
    
    /* Register SIGINT handler */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("    signal(SIGINT)");
    } else {
        printf("    ✓ SIGINT handler registered (Ctrl+C)\n");
    }
    
    /* Register SIGTERM handler */
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("    signal(SIGTERM)");
    } else {
        printf("    ✓ SIGTERM handler registered\n");
    }
    
    /* Register SIGHUP handler */
    if (signal(SIGHUP, sighup_handler) == SIG_ERR) {
        perror("    signal(SIGHUP)");
    } else {
        printf("    ✓ SIGHUP handler registered\n");
    }
    
    /* Register SIGSEGV handler */
    if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
        perror("    signal(SIGSEGV)");
    } else {
        printf("    ✓ SIGSEGV handler registered\n");
    }
    
    /* Ignore SIGPIPE (common for network servers) */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("    signal(SIGPIPE)");
    } else {
        printf("    ✓ SIGPIPE ignored (won't crash on broken pipe)\n");
    }
    
    printf("\n  Common signals:\n");
    printf("    SIGINT  (2)  - Interrupt from keyboard (Ctrl+C)\n");
    printf("    SIGTERM (15) - Termination request\n");
    printf("    SIGHUP  (1)  - Hangup (often used for reload)\n");
    printf("    SIGSEGV (11) - Segmentation fault\n");
    printf("    SIGPIPE (13) - Broken pipe\n");
    printf("    SIGALRM (14) - Alarm timer\n");
    printf("    SIGCHLD (17) - Child process terminated\n");
    
    printf("\n  ⚠️  Try pressing Ctrl+C to test SIGINT handler!\n");
    printf("     (Program will continue after handling)\n");
}

/* ========== SECTION 2: USING SIGACTION (PREFERRED) ========== */

/* Signal handler with sigaction */
void sigaction_handler(int sig, siginfo_t *info, void *ucontext) {
    /* siginfo_t provides more information about the signal */
    const char *signame;
    switch (sig) {
        case SIGINT: signame = "SIGINT"; break;
        case SIGTERM: signame = "SIGTERM"; break;
        case SIGHUP: signame = "SIGHUP"; break;
        case SIGSEGV: signame = "SIGSEGV"; break;
        default: signame = "UNKNOWN"; break;
    }
    
    char msg[100];
    int len = snprintf(msg, sizeof(msg), 
                       "\n  [SIGACTION] Caught %s (si_pid=%d)\n", 
                       signame, info->si_pid);
    write(STDERR_FILENO, msg, len);
    
    /* Set flag for main code */
    if (sig == SIGINT) got_sigint = 1;
    if (sig == SIGTERM) got_sigterm = 1;
}

void demo_sigaction(void) {
    puts("\n[2] Using sigaction() (Preferred Method)");
    
    printf("  sigaction() vs signal():\n");
    printf("    • signal(): Simple, but behavior varies by system\n");
    printf("    • sigaction(): POSIX standard, more control, recommended\n\n");
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    
    /* Set handler function */
    sa.sa_sigaction = sigaction_handler;
    
    /* Use sa_sigaction instead of sa_handler */
    sa.sa_flags = SA_SIGINFO;  /* Get siginfo_t with details */
    
    /* Block other signals while handler runs */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);
    
    /* Register for SIGINT */
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("    sigaction(SIGINT)");
    } else {
        printf("    ✓ SIGINT handler registered with sigaction()\n");
        printf("    • SA_SIGINFO flag: Get siginfo_t details\n");
        printf("    • sa_mask: Block SIGINT/SIGTERM during handler\n");
    }
    
    printf("\n  sigaction() benefits:\n");
    printf("    ✓ Portable across Unix systems\n");
    printf("    ✓ Can get signal sender info (siginfo_t)\n");
    printf("    ✓ Can block signals during handler execution\n");
    printf("    ✓ Can control handler behavior with flags\n");
}

/* ========== SECTION 3: SIGNAL SAFETY ========== */

/* Async-signal-safe flag variable */
static volatile sig_atomic_t shutdown_requested = 0;

/* Safe signal handler - only sets flag */
void safe_shutdown_handler(int sig) {
    shutdown_requested = 1;
    /* That's it! No printf, no malloc, no complex logic */
}

/* Main loop that checks flag */
void run_main_loop(void) {
    printf("  Main loop running (press Ctrl+C to shutdown)...\n");
    printf("  Loop iteration: ");
    
    int iteration = 0;
    while (!shutdown_requested && iteration < 10) {
        printf("%d ", iteration++);
        fflush(stdout);
        sleep(1);  /* Safe to sleep in main code */
    }
    
    printf("\n  Main loop exited\n");
}

/* Demonstrate unsafe vs safe */
void demo_signal_safety(void) {
    puts("\n[3] Signal Safety");
    
    printf("  Async-signal-safe functions (can use in handlers):\n");
    printf("    • _exit(), exit() (but _exit is safer)\n");
    printf("    • write(), read()\n");
    printf("    • signal(), sigaction()\n");
    printf("    • kill(), raise()\n");
    printf("    • alarm(), sleep()\n");
    printf("    • sigprocmask(), sigemptyset()\n");
    printf("    • And about 100 more (see man 7 signal-safety)\n\n");
    
    printf("  NOT async-signal-safe (DON'T use in handlers):\n");
    printf("    • printf(), fprintf(), sprintf()\n");
    printf("    • malloc(), free(), realloc()\n");
    printf("    • strcpy(), strlen(), etc.\n");
    printf("    • Most stdio functions\n");
    printf("    • Most library functions\n\n");
    
    printf("  Why? These functions are not reentrant!\n");
    printf("    • They may use static buffers\n");
    printf("    • They may call malloc internally\n");
    printf("    • They may be interrupted mid-execution\n");
    printf("    • Can cause deadlocks or corruption\n\n");
    
    printf("  Safe pattern:\n");
    printf("    1. Handler sets volatile sig_atomic_t flag\n");
    printf("    2. Main code checks flag periodically\n");
    printf("    3. Main code does complex cleanup\n\n");
    
    /* Register safe handler */
    if (signal(SIGINT, safe_shutdown_handler) == SIG_ERR) {
        perror("    signal()");
        return;
    }
    
    /* Run main loop */
    run_main_loop();
    
    /* Safe cleanup in main code (not in handler!) */
    if (shutdown_requested) {
        printf("  Performing graceful shutdown...\n");
        printf("    • Close files\n");
        printf("    • Free memory\n");
        printf("    • Save state\n");
        printf("    • Cleanup complete\n");
    }
}

/* ========== SECTION 4: SIGNAL MASKING ========== */

void demo_signal_masking(void) {
    puts("\n[4] Signal Masking");
    
    sigset_t mask, old_mask;
    
    printf("  Signal mask operations:\n\n");
    
    /* Initialize mask */
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    
    printf("  1. Block SIGINT and SIGTERM:\n");
    if (sigprocmask(SIG_BLOCK, &mask, &old_mask) == -1) {
        perror("      sigprocmask");
    } else {
        printf("      ✓ Signals blocked\n");
        printf("      • SIGINT/SIGTERM will be queued (pending)\n");
        printf("      • Will be delivered when unblocked\n");
    }
    
    printf("\n  2. Simulate work with signals blocked:\n");
    printf("      Working for 2 seconds (signals blocked)...\n");
    sleep(2);
    printf("      ✓ Work complete\n");
    
    printf("\n  3. Restore old mask (unblock signals):\n");
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1) {
        perror("      sigprocmask");
    } else {
        printf("      ✓ Signals unblocked\n");
        printf("      • Pending signals will be delivered now\n");
    }
    
    printf("\n  4. Check pending signals:\n");
    sigset_t pending;
    if (sigpending(&pending) == -1) {
        perror("      sigpending");
    } else {
        if (sigismember(&pending, SIGINT)) {
            printf("      • SIGINT is pending\n");
        }
        if (sigismember(&pending, SIGTERM)) {
            printf("      • SIGTERM is pending\n");
        }
        printf("      (No pending signals if none were sent)\n");
    }
    
    printf("\n  Signal mask use cases:\n");
    printf("    • Critical sections (prevent interruption)\n");
    printf("    • Atomic operations\n");
    printf("    • Prevent signal storms\n");
    printf("    • Controlled signal delivery\n");
}

/* ========== SECTION 5: ALARM TIMERS ========== */

static volatile sig_atomic_t alarm_fired = 0;

void alarm_handler(int sig) {
    alarm_fired = 1;
    const char msg[] = "\n  [ALARM] Timer expired!\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

void demo_alarm_timers(void) {
    puts("\n[5] Alarm Timers (SIGALRM)");
    
    /* Register alarm handler */
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("    signal(SIGALRM)");
        return;
    }
    
    printf("  Setting alarm for 3 seconds...\n");
    alarm(3);  /* Send SIGALRM in 3 seconds */
    
    printf("  Waiting for alarm (press Ctrl+C to interrupt)...\n");
    
    /* Wait for alarm or interrupt */
    while (!alarm_fired && !got_sigint) {
        pause();  /* Sleep until signal */
    }
    
    if (alarm_fired) {
        printf("  ✓ Alarm fired after 3 seconds\n");
    }
    
    printf("\n  Alarm use cases:\n");
    printf("    • Timeout for blocking operations\n");
    printf("    • Periodic tasks\n");
    printf("    • Watchdog timers\n");
    printf("    • Rate limiting\n");
    
    /* Cancel any pending alarm */
    alarm(0);
}

/* ========== SECTION 6: GRACEFUL SHUTDOWN ========== */

typedef struct {
    bool running;
    int cleanup_count;
} AppState;

static AppState app_state = {0};

void graceful_shutdown(void) {
    printf("  Graceful shutdown initiated...\n");
    app_state.running = false;
    
    /* Cleanup in main code (safe, not in signal handler!) */
    printf("  Cleanup step %d: Close files\n", ++app_state.cleanup_count);
    printf("  Cleanup step %d: Free memory\n", ++app_state.cleanup_count);
    printf("  Cleanup step %d: Save state\n", ++app_state.cleanup_count);
    printf("  Cleanup step %d: Notify peers\n", ++app_state.cleanup_count);
    
    printf("  ✓ Shutdown complete\n");
}

void demo_graceful_shutdown(void) {
    puts("\n[6] Graceful Shutdown Pattern");
    
    /* Register handlers */
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    
    app_state.running = true;
    app_state.cleanup_count = 0;
    
    printf("  Application running (send SIGTERM to test)...\n");
    printf("  PID: %d\n", getpid());
    printf("  Send signal with: kill -TERM %d\n", getpid());
    printf("\n  Waiting for shutdown signal (or 5 second timeout)...\n");
    
    /* Main loop with shutdown check */
    time_t start_time = time(NULL);
    while (app_state.running && !got_sigterm && !got_sigint) {
        /* Check timeout */
        if (time(NULL) - start_time > 5) {
            printf("  Timeout reached, initiating shutdown...\n");
            break;
        }
        
        /* Do work */
        printf("  .");
        fflush(stdout);
        sleep(1);
    }
    
    /* Graceful shutdown */
    graceful_shutdown();
}

/* ========== SECTION 7: CHILD PROCESS SIGNALS ========== */

void sigchld_handler(int sig) {
    const char msg[] = "\n  [SIGCHLD] Child process terminated\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

void demo_child_signals(void) {
    puts("\n[7] Child Process Signals (SIGCHLD)");
    
    /* Register SIGCHLD handler */
    if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
        perror("    signal(SIGCHLD)");
        return;
    }
    
    printf("  Creating child process...\n");
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("    fork()");
        return;
    } else if (pid == 0) {
        /* Child process */
        printf("    Child process (PID: %d) running...\n", getpid());
        sleep(2);
        printf("    Child process exiting\n");
        exit(0);
    } else {
        /* Parent process */
        printf("    Parent process (PID: %d)\n", getpid());
        printf("    Child PID: %d\n", pid);
        printf("    Waiting for child to terminate...\n");
        
        /* Wait for child */
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("    ✓ Child exited with status: %d\n", WEXITSTATUS(status));
        }
    }
    
    printf("\n  SIGCHLD use cases:\n");
    printf("    • Reap zombie processes\n");
    printf("    • Monitor child processes\n");
    printf("    • Implement process pools\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] Signal Handling Best Practices");
    
    printf("  1. Use sigaction() instead of signal():\n");
    printf("     More portable, more control\n\n");
    
    printf("  2. Keep signal handlers minimal:\n");
    printf("     Set flag, return immediately\n");
    printf("     Do complex work in main code\n\n");
    
    printf("  3. Use volatile sig_atomic_t for flags:\n");
    printf("     volatile sig_atomic_t flag = 0;\n\n");
    
    printf("  4. Only call async-signal-safe functions:\n");
    printf("     write(), _exit(), signal(), etc.\n");
    printf("     NO printf(), malloc(), free()!\n\n");
    
    printf("  5. Preserve errno in handlers:\n");
    printf("     int saved_errno = errno;\n");
    printf("     /* handler code */\n");
    printf("     errno = saved_errno;\n\n");
    
    printf("  6. Block signals during critical sections:\n");
    printf("     sigprocmask(SIG_BLOCK, &mask, &old);\n");
    printf("     /* critical section */\n");
    printf("     sigprocmask(SIG_SETMASK, &old, NULL);\n\n");
    
    printf("  7. Handle SIGPIPE explicitly:\n");
    printf("     signal(SIGPIPE, SIG_IGN);\n");
    printf("     Prevents crash on broken pipe\n\n");
    
    printf("  8. Implement graceful shutdown:\n");
    printf("     Catch SIGTERM/SIGINT\n");
    printf("     Cleanup resources properly\n");
    printf("     Exit cleanly\n\n");
    
    printf("  9. Document signal behavior:\n");
    printf("     Which signals are handled?\n");
    printf("     What's the expected behavior?\n\n");
    
    printf("  10. Test signal handling:\n");
    printf("      Send signals manually (kill command)\n");
    printf("      Test edge cases (rapid signals, etc.)\n");
}

/* ========== SECTION 9: PRACTICAL EXAMPLES ========== */

/* Timeout wrapper for blocking operations */
int operation_with_timeout(void (*operation)(void), int timeout_secs) {
    volatile sig_atomic_t timed_out = 0;
    
    void timeout_handler(int sig) {
        timed_out = 1;
    }
    
    /* Set up alarm */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = timeout_handler;
    sigaction(SIGALRM, &sa, NULL);
    
    alarm(timeout_secs);
    
    /* Run operation */
    operation();
    
    /* Cancel alarm */
    alarm(0);
    
    return timed_out ? -1 : 0;
}

/* Reload config on SIGHUP */
void reload_config(void) {
    printf("    Reloading configuration...\n");
    /* In real code: re-read config file, update state */
    printf("    ✓ Configuration reloaded\n");
}

void demo_practical_examples(void) {
    puts("\n[9] Practical Signal Examples");
    
    printf("  Example 1: Timeout for blocking operation\n");
    printf("    • Set SIGALRM handler\n");
    printf("    • Call alarm(timeout)\n");
    printf("    • Run blocking operation\n");
    printf("    • Cancel alarm on success\n");
    printf("    • Handle timeout in signal handler\n\n");
    
    printf("  Example 2: Config reload on SIGHUP\n");
    printf("    • Register SIGHUP handler\n");
    printf("    • Handler sets reload_flag\n");
    printf("    • Main loop checks flag\n");
    printf("    • Reload config when flag set\n\n");
    
    printf("  Example 3: Daemon control\n");
    printf("    • SIGTERM: Graceful shutdown\n");
    printf("    • SIGHUP: Reload configuration\n");
    printf("    • SIGUSR1: Dump stats/debug info\n");
    printf("    • SIGUSR2: Custom application signal\n\n");
    
    printf("  Example 4: Zombie prevention\n");
    printf("    • Register SIGCHLD handler\n");
    printf("    • Call waitpid() in handler\n");
    printf("    • Prevent zombie processes\n\n");
    
    /* Demo config reload */
    printf("  Simulating config reload:\n");
    reload_config();
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 39: Signal Handling ===");
    puts("Systems Programming: Signals, handlers, safety, masks, real-world patterns\n");
    
    /* Section 1: Basic handlers */
    demo_basic_handlers();
    
    /* Section 2: sigaction */
    demo_sigaction();
    
    /* Section 3: Signal safety */
    demo_signal_safety();
    
    /* Section 4: Signal masking */
    demo_signal_masking();
    
    /* Section 5: Alarm timers */
    demo_alarm_timers();
    
    /* Section 6: Graceful shutdown */
    demo_graceful_shutdown();
    
    /* Section 7: Child process signals */
    demo_child_signals();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* Section 9: Practical examples */
    demo_practical_examples();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 39: Signal Handling complete!");
    puts("Key takeaways:");
    puts("  • Signals are OS notifications to processes");
    puts("  • Use sigaction() instead of signal()");
    puts("  • Keep signal handlers minimal (set flag only)");
    puts("  • Only use async-signal-safe functions in handlers");
    puts("  • Use volatile sig_atomic_t for flags");
    puts("  • Block signals during critical sections");
    puts("  • Implement graceful shutdown (SIGTERM/SIGINT)");
    puts("  • Handle SIGCHLD to prevent zombies");
    puts("  • Use SIGALRM for timeouts");
    puts("  • Test signal handling thoroughly!");
    puts("\n🎓 You're handling signals like a pro!");
    puts("   Signal handling is essential for robust systems code! 🚀\n");
    
    return EXIT_SUCCESS;
}