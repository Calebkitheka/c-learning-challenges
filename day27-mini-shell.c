/*
 * File: day27-mini-shell.c
 * Goal: Implement a mini command-line shell (Chapter 28)
 * Concepts: fork, exec, wait, command parsing, built-in commands, process management
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o mini_shell day27-mini-shell.c
 * 
 * Note: This uses POSIX functions (fork, exec, wait) - works on Linux/macOS/WSL
 * For Windows, would need different process creation APIs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>

/* ========== CONFIGURATION ========== */
#define MAX_CMD_LEN 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100
#define SHELL_NAME "mini-shell"
#define SHELL_VERSION "1.0.0"

/* ========== GLOBAL STATE ========== */
static char *command_history[MAX_HISTORY];
static int history_count = 0;
static int history_index = 0;
static bool running = true;

/* ========== FUNCTION PROTOTYPES ========== */
char *read_line(void);
char **split_line(char *line);
void add_to_history(const char *cmd);
void print_history(void);
void free_args(char **args);

/* Built-in command handlers */
bool cmd_cd(char **args);
bool cmd_exit(char **args);
bool cmd_help(char **args);
bool cmd_history_cmd(char **args);
bool cmd_pwd_cmd(char **args);
bool cmd_echo_cmd(char **args);
bool cmd_clear_cmd(char **args);

/* ========== SECTION 1: COMMAND LINE READING ========== */

/* Read a line of input from stdin */
char *read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;
    
    /* Print prompt */
    printf("\n%s> ", SHELL_NAME);
    fflush(stdout);
    
    /* Read line using getline (auto-allocates buffer) */
    ssize_t chars_read = getline(&line, &bufsize, stdin);
    
    if (chars_read == -1) {
        /* EOF (Ctrl+D) or error */
        if (feof(stdin)) {
            printf("\n");
            free(line);
            return NULL;
        }
        perror("getline");
        free(line);
        return NULL;
    }
    
    /* Remove trailing newline */
    if (chars_read > 0 && line[chars_read - 1] == '\n') {
        line[chars_read - 1] = '\0';
    }
    
    return line;
}

/* ========== SECTION 2: COMMAND PARSING ========== */

/* Split command line into arguments */
char **split_line(char *line) {
    if (!line) return NULL;
    
    /* Allocate array for arguments */
    char **args = malloc(MAX_ARGS * sizeof(char*));
    if (!args) {
        perror("malloc");
        return NULL;
    }
    
    int arg_count = 0;
    char *token;
    char *saveptr;
    
    /* Tokenize by whitespace */
    token = strtok_r(line, " \t\r\n", &saveptr);
    
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        /* Skip comments */
        if (token[0] == '#') {
            break;
        }
        
        args[arg_count++] = token;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }
    
    /* Null-terminate the array */
    args[arg_count] = NULL;
    
    return args;
}

/* Free argument array */
void free_args(char **args) {
    if (args) {
        free(args);
    }
}

/* ========== SECTION 3: COMMAND HISTORY ========== */

/* Add command to history */
void add_to_history(const char *cmd) {
    if (!cmd || strlen(cmd) == 0) return;
    
    /* Don't add duplicate of last command */
    if (history_count > 0 && strcmp(cmd, command_history[history_count - 1]) == 0) {
        return;
    }
    
    /* Allocate and copy */
    char *copy = strdup(cmd);
    if (!copy) return;
    
    /* Add to history */
    if (history_count < MAX_HISTORY) {
        command_history[history_count++] = copy;
    } else {
        /* Remove oldest */
        free(command_history[0]);
        memmove(&command_history[0], &command_history[1], 
                (MAX_HISTORY - 1) * sizeof(char*));
        command_history[MAX_HISTORY - 1] = copy;
    }
    
    history_index = history_count;
}

/* Print command history */
void print_history(void) {
    printf("  Command History:\n");
    for (int i = 0; i < history_count; i++) {
        printf("    %4d  %s\n", i + 1, command_history[i]);
    }
}

/* ========== SECTION 4: BUILT-IN COMMANDS ========== */

/* cd: Change directory */
bool cmd_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "  %s: cd requires an argument\n", SHELL_NAME);
        return false;
    }
    
    if (chdir(args[1]) != 0) {
        perror("  cd");
        return false;
    }
    
    return true;
}

/* exit: Exit shell */
bool cmd_exit(char **args) {
    printf("  Goodbye!\n");
    running = false;
    return true;
}

/* help: Show help message */
bool cmd_help(char **args) {
    printf("\n");
    printf("  %s version %s - Mini Shell Help\n", SHELL_NAME, SHELL_VERSION);
    printf("  =================================\n\n");
    printf("  Built-in Commands:\n");
    printf("    cd <path>      - Change current directory\n");
    printf("    pwd            - Print working directory\n");
    printf("    echo <text>    - Print text to stdout\n");
    printf("    history        - Show command history\n");
    printf("    clear          - Clear screen\n");
    printf("    help           - Show this help message\n");
    printf("    exit           - Exit the shell\n\n");
    printf("  Usage:\n");
    printf("    Type a command and press Enter\n");
    printf("    Use Ctrl+C to interrupt running commands\n");
    printf("    Use Ctrl+D to exit the shell\n\n");
    printf("  Examples:\n");
    printf("    %s> ls -la\n", SHELL_NAME);
    printf("    %s> cd /tmp\n", SHELL_NAME);
    printf("    %s> echo Hello World\n", SHELL_NAME);
    printf("    %s> help\n", SHELL_NAME);
    printf("    %s> exit\n", SHELL_NAME);
    printf("\n");
    
    return true;
}

/* history: Show command history */
bool cmd_history_cmd(char **args) {
    print_history();
    return true;
}

/* pwd: Print working directory */
bool cmd_pwd_cmd(char **args) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("  %s\n", cwd);
        return true;
    } else {
        perror("  pwd");
        return false;
    }
}

/* echo: Print arguments */
bool cmd_echo_cmd(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s", args[i]);
        if (args[i + 1] != NULL) {
            printf(" ");
        }
    }
    printf("\n");
    return true;
}

/* clear: Clear screen */
bool cmd_clear_cmd(char **args) {
    /* ANSI escape code to clear screen */
    printf("\033[2J\033[H");
    fflush(stdout);
    return true;
}

/* ========== SECTION 5: BUILT-IN COMMAND TABLE ========== */

/* Built-in command structure */
typedef struct {
    const char *name;
    bool (*handler)(char **);
    const char *description;
} BuiltinCommand;

/* Built-in command table */
static BuiltinCommand builtins[] = {
    {"cd", cmd_cd, "Change directory"},
    {"exit", cmd_exit, "Exit shell"},
    {"help", cmd_help, "Show help"},
    {"history", cmd_history_cmd, "Show command history"},
    {"pwd", cmd_pwd_cmd, "Print working directory"},
    {"echo", cmd_echo_cmd, "Print text"},
    {"clear", cmd_clear_cmd, "Clear screen"},
    {NULL, NULL, NULL}  /* Sentinel */
};

/* Check if command is built-in and execute */
bool execute_builtin(char **args) {
    if (!args || !args[0]) return false;
    
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(args[0], builtins[i].name) == 0) {
            return builtins[i].handler(args);
        }
    }
    
    return false;  /* Not a built-in */
}

/* ========== SECTION 6: EXTERNAL COMMAND EXECUTION ========== */

/* Launch external command */
bool launch_external_command(char **args) {
    if (!args || !args[0]) return false;
    
    /* Fork process */
    pid_t pid = fork();
    
    if (pid < 0) {
        /* Fork failed */
        perror("  fork");
        return false;
    } else if (pid == 0) {
        /* Child process */
        
        /* Execute command */
        if (execvp(args[0], args) == -1) {
            /* exec failed */
            fprintf(stderr, "  %s: command not found: %s\n", SHELL_NAME, args[0]);
            exit(EXIT_FAILURE);
        }
        
        /* Should never reach here */
        exit(EXIT_SUCCESS);
    } else {
        /* Parent process */
        
        /* Wait for child to complete */
        int status;
        waitpid(pid, &status, 0);
        
        /* Check exit status */
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                return false;  /* Command failed */
            }
        } else if (WIFSIGNALED(status)) {
            /* Process was killed by signal */
            int sig = WTERMSIG(status);
            printf("  Command killed by signal %d\n", sig);
            return false;
        }
        
        return true;  /* Command succeeded */
    }
}

/* ========== SECTION 7: MAIN SHELL LOOP ========== */

/* Main shell loop */
void shell_loop(void) {
    char *line;
    char **args;
    bool is_builtin;
    
    printf("\n");
    printf("  ╔════════════════════════════════════════════╗\n");
    printf("  ║  %s v%s                        ║\n", SHELL_NAME, SHELL_VERSION);
    printf("  ║  Type 'help' for available commands        ║\n");
    printf("  ║  Type 'exit' or Ctrl+D to quit             ║\n");
    printf("  ╚════════════════════════════════════════════╝\n");
    
    while (running) {
        /* Read command line */
        line = read_line();
        
        if (line == NULL) {
            /* EOF or error */
            printf("\n");
            break;
        }
        
        /* Skip empty lines */
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        
        /* Add to history */
        add_to_history(line);
        
        /* Parse command */
        args = split_line(line);
        
        if (!args || !args[0]) {
            free(line);
            free_args(args);
            continue;
        }
        
        /* Try built-in commands first */
        is_builtin = execute_builtin(args);
        
        if (!is_builtin) {
            /* Execute external command */
            launch_external_command(args);
        }
        
        /* Cleanup */
        free(line);
        free_args(args);
    }
}

/* ========== SECTION 8: CLEANUP ========== */

/* Free all resources */
void cleanup(void) {
    for (int i = 0; i < history_count; i++) {
        free(command_history[i]);
    }
}

/* Signal handler for Ctrl+C */
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n");
        /* Don't exit shell, just interrupt current command */
    }
}

/* ========== MAIN: ENTRY POINT ========== */
int main(void) {
    /* Setup signal handler */
    signal(SIGINT, signal_handler);
    
    /* Run shell */
    shell_loop();
    
    /* Cleanup */
    cleanup();
    
    printf("  Shell terminated.\n");
    
    return EXIT_SUCCESS;
}