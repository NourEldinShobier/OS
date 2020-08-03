#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdbool.h>

char *scanString() {
    char *Text, Input;
    size_t CH = 1, Index = 0;

    Text = malloc(sizeof(char) * CH);
    if (!Text) return NULL;

    while ((Input = getchar()) != '\n' && Input != EOF) {

        Text = realloc(Text, CH++ * sizeof(char));
        if (!Text) return NULL;

        Text[Index++] = Input;
    }

    Text[Index] = '\0';

    if (Text[0] == '\0') {
        free(Text);
        return NULL;
    }

    return Text;
}

void handler(int sig) {
    FILE *file = fopen("log.txt", "a");

    fprintf(file, "child died\n");
    fclose(file);
}

int main(void) {
    while (1) {
        char *input, *pointer = NULL;
        char *params[5] = {NULL, NULL, NULL, NULL, NULL};

        int paramsLength = 0, zombie = false;

        printf("%s@shell> ", getenv("USER"));

        // 1) scan input
        input = scanString();

        // 2) check if zombie
        if (strstr(input, "&") != NULL) {
            zombie = true;
        }

        // 3) parse command
        pointer = strtok(input, " ");

        // 4) parse params
        while (pointer != NULL) {
            params[paramsLength++] = pointer;
            pointer = strtok(NULL, " ");
        }

        if (strcmp(input, "exit") == 0) exit(0);
        else {
            int pid = fork();

            if (pid > 0) { // parent

                signal(SIGCHLD, handler);
                if (zombie == false) wait(0);
            }

            if (pid == 0) { // child
                execvp(params[0], params);
            }
        }
    }
}