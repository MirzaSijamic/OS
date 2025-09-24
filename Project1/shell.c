#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <limits.h>

#define INPUT_MAX 1024
#define MAX_ARGS 64

// Define ANSI color codes for easy use
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"


void display_welcome_message() {
    // Clear the screen for a clean welcome message
    printf("\033[H\033[J"); 
    
    // Print the Swedish Flag ASCII Art with colors
    printf("========================================\n");
    printf(COLOR_BLUE "||||||||||||||||||||||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||" COLOR_YELLOW " + + + " COLOR_BLUE "||||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||" COLOR_YELLOW " + + + " COLOR_BLUE "||||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_YELLOW "+++++++++++ + + + +++++++++++++++++++++" COLOR_RESET "\n");
    printf(COLOR_YELLOW "+++++++++++ + + + +++++++++++++++++++++" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||" COLOR_YELLOW " + + + " COLOR_BLUE "||||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||" COLOR_YELLOW " + + + " COLOR_BLUE "||||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "||||||||||||||||||||||||||||||||||||||||" COLOR_RESET "\n");
    
    // Print the Welcome Text in a different color
    printf("\nWelcome to " COLOR_CYAN "SweBiH Shell" COLOR_RESET "!\n");
    printf("Your friendly shell for coding adventures.\n\n");
    

    printf(COLOR_BLUE "|" COLOR_WHITE "o" COLOR_YELLOW " * * * * * * * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||" COLOR_WHITE "o" COLOR_YELLOW " * * * * * * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||" COLOR_WHITE "o" COLOR_YELLOW " * * * * * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||" COLOR_WHITE "o" COLOR_YELLOW " * * * * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||" COLOR_WHITE "o" COLOR_YELLOW " * * * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||" COLOR_WHITE "o" COLOR_YELLOW " * * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||||" COLOR_WHITE "o" COLOR_YELLOW " * * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf(COLOR_BLUE "|||||||||||||||" COLOR_WHITE "o" COLOR_YELLOW " * " COLOR_BLUE "|||||||||||||||||||||" COLOR_RESET "\n");
    printf("========================================\n\n");
    
    fflush(stdout); // Ensure the message is printed immediately

    }

void display_promt(){
    char current_working_directory[INPUT_MAX];
    char username[INPUT_MAX];
    char hostname[INPUT_MAX];

    char *home_directory = getenv("HOME");
    getcwd(current_working_directory, sizeof(current_working_directory));
    getlogin_r(username, sizeof(username));
    gethostname(hostname, sizeof(hostname));

    if(strncmp(current_working_directory, home_directory, strlen(home_directory))){
        printf("%s@%s:~%s$", username, hostname, current_working_directory + strlen(home_directory));
    }
    else printf("%s@%s:~%s$", username, hostname, current_working_directory);

    fflush(stdout);

}


int builtin_du(char **args) {
    int human_readable = 0;
    int max_depth = -1;
    int i = 1;
    char *target; // Target is determined first now.

    // --- 1. Argument Parsing (MUST be done first) ---
    while (args[i] != NULL && args[i][0] == '-') {
        if (strcmp(args[i], "-h") == 0) {
            human_readable = 1;
        } else if (strcmp(args[i], "-d") == 0 && args[i+1] != NULL) {
            max_depth = atoi(args[i+1]);
            i++; // Skip the number
        }
        i++;
    }

    // --- 2. Determine the Target Path ---
    if (args[i] != NULL) {
        target = args[i];
    } else {
        target = ".";
    }

    // --- 3. Now, try to open the target and handle both cases ---
    long total_size = 0;
    struct stat statbuf;
    DIR *dir = opendir(target);

    if (dir == NULL) {
        // It's a file or an error.
        if (lstat(target, &statbuf) == 0) {
            total_size = statbuf.st_blocks; // Get size of the file.
        } else {
            // A real error occurred (e.g., file not found, permission denied).
            // Using target in perror gives a more useful message.
            perror(target);
            return 1; // Return an error code.
        }
    } else {
        // It's a directory. Proceed with reading its contents.
        struct dirent *entry;

        // Get the size of the directory entry itself.
        if (lstat(target, &statbuf) == 0) {
            total_size += statbuf.st_blocks;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", target, entry->d_name);

            if (lstat(path, &statbuf) != 0) {
                perror(path); // Print the path that caused the error.
                continue;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                if (max_depth != 0) {
                    char **new_args = malloc(5 * sizeof(char*));
                    int j = 0;
                    new_args[j++] = "du";

                    if (human_readable) new_args[j++] = "-h";
                    
                    char depth_str[12];
                    if (max_depth > 0) {
                        new_args[j++] = "-d";
                        snprintf(depth_str, sizeof(depth_str), "%d", max_depth - 1);
                        new_args[j++] = depth_str;
                    }
                    
                    new_args[j++] = path;
                    new_args[j] = NULL;

                    total_size += builtin_du(new_args);
                    free(new_args);
                }
            } else {
                total_size += statbuf.st_blocks;
            }
        }
        closedir(dir);
    }

    // --- Final Print (shared by both file and directory paths) ---
    long final_kb = total_size / 2;
    if (strcmp(args[0], "du") == 0 && (args[1] == target || args[1] == NULL || args[1][0] == '-')) {
        if (human_readable) {
            if (final_kb > 1024) {
                printf("%.1fM\t%s\n", final_kb / 1024.0, target);
            } else {
                printf("%ldK\t%s\n", final_kb, target);
            }
        } else {
            printf("%ld\t%s\n", final_kb, target);
        }
    }

    return total_size;
}

int builtin_date(char **args) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char buffer[80];
    int custom_format = 0;
    char *format = "%a %b %d %H:%M:%S %Y";
    if (args[1] != NULL && strcmp(args[1], "-f") == 0 && args[2] != NULL) {
        format = args[2];
        custom_format = 1;
    }
    strftime(buffer, sizeof(buffer), format, tm);
    printf("%s\n", buffer);
    return 0;
}

int builtin_rev(char **args) {
    if (args[1] == NULL) {
        char line[INPUT_MAX];
        while (fgets(line, sizeof(line), stdin)) {
            line[strcspn(line, "\n")] = '\0';
            int len = strlen(line);
            for (int i = len - 1; i >= 0; i--) putchar(line[i]);
            putchar('\n');
        }
    } else {
        for (int i = 1; args[i] != NULL; i++) {
            int len = strlen(args[i]);
            for (int j = len - 1; j >= 0; j--) putchar(args[i][j]);
            putchar(' ');
        }
        putchar('\n');
    }
    return 0;
}

int builtin_cd(char **args) {
    if (args[1] == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: No home directory found\n");
            return 1;
        }
        if (chdir(home) != 0) {
            perror("cd");
            return 1;
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return 1;
        }
    }
    return 0;
}

int parse_input(char *input, char **args) {
    int i=0;
    char *token = strtok(input, " \t");
    while (token!=NULL && i<(MAX_ARGS-1)){
        args[i++] = token;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
    return i;
}

void handle_pipelining(char **args) {
    int i = 0;
    int num_cmds = 0;
    char *commands[4][MAX_ARGS]; // support up to 4 commands in pipeline

    // Split args into commands separated by "|"
    int cmd_index = 0, arg_index = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "|") == 0) {
            commands[cmd_index][arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
        } else {
            commands[cmd_index][arg_index++] = args[i];
        }
        i++;
    }
    commands[cmd_index][arg_index] = NULL;
    num_cmds = cmd_index + 1;

    int fd[2], in_fd = 0;

    for (i = 0; i < num_cmds; i++) {
        pipe(fd);
        pid_t pid = fork();

        if (pid == 0) {
            dup2(in_fd, 0); // set stdin to previous pipe read end

            if (i < num_cmds - 1) {
                dup2(fd[1], 1); // set stdout to pipe write end
            }

            close(fd[0]);
            execvp(commands[i][0], commands[i]);
            perror("execvp");
            exit(1);
        } else {
            wait(NULL);
            close(fd[1]);
            in_fd = fd[0]; // next command reads from this
        }
    }
}

int builtin_exit(char **args) {
    printf( "Have a nice day and Goodbye!\n" );
    exit(0);
    return 0;
}

int builtin_mv(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "SweBIH: mv expects 2 arguments\n");
        return 1;
    }
    if (rename(args[1], args[2]) != 0) {
        perror("SweBIH");
        return 1;
    }
    return 0;
}

int builtin_forkbomb(char **args) {
    printf( COLOR_YELLOW "WARNING: FORKBOMB DEMONSTRATION\n" COLOR_RESET);
    printf("This will create processes exponentially.\n");
    printf("Press Ctrl+C within 5 seconds to abort...\n");
    sleep(5);
    
    printf(COLOR_YELLOW "Forkbomb activated!\n" COLOR_RESET);
    while (1) {
        fork();
    }
    return 0;
}

void execute_command(char *cmd, char **cmd_args){
    if (cmd == NULL) printf("error\n");
    else if(strncmp(cmd, "cd", 2) == 0){
        builtin_cd(cmd_args);
    }
    else if(strncmp(cmd, "exit", 4) == 0){
        builtin_exit(cmd_args);
    }
    else if(strncmp(cmd, "mv", 2) == 0){
        builtin_mv(cmd_args);
    }
    else if(strncmp(cmd, "du", 2) == 0)
    {
        builtin_du(cmd_args);
    }
    else if(strncmp(cmd, "date", 4) == 0){
        builtin_date(cmd_args);
    }
    else if(strncmp(cmd, "rev", 3) == 0){
        builtin_rev(cmd_args);
    }
    else if(strncmp(cmd, "forkbomb", 8) == 0){
        builtin_forkbomb(cmd_args);
    }
    else{
        pid_t pid = fork();
        if (pid == 0) {
            int has_pipe = 0;
            for (int i = 0; cmd_args[i] != NULL; i++) {
                if (strcmp(cmd_args[i], "|") == 0) {
                    has_pipe = 1;
                    break;
                }
            }

            if (has_pipe) {
                handle_pipelining(cmd_args);
                exit(0);
            } else {
                if (execvp(cmd, cmd_args) == -1) {
                    printf("SweBIH: command not found: %s\n", cmd);
                }
            }
            exit(EXIT_FAILURE);
        }

        else if (pid < 0) {
            perror("SweBIH");
        } 

        else {
            int status;
            waitpid(pid, &status, 0);
        }
    } 
    //printf("nesta\n"); //mv, du, date, rev

}


int main() {
    char input[INPUT_MAX];
    char **args = malloc(MAX_ARGS * sizeof(char*));
    char *input_pointer = &input[0];

    //display_promt();

    display_welcome_message();

    while(1){
        display_promt();

        fgets(input, INPUT_MAX, stdin);
        input[strcspn(input, "\n")] = '\0';

        //fflush(stdout);

        int args_read = parse_input(input_pointer, args);
        
        /*
        printf("Read %d args\n", args_read);
        for(int i = 0; i<args_read; i++){
            printf("arg[%d] = %s\n", i, args[i]);
        }*/

        char *cmd = args[0];
        char **cmd_args = args;
        execute_command(cmd, cmd_args);

    }    
    return 0;
}