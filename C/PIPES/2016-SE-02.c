#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>
#include <sys/wait.h>
#include <stdio.h>

char prompt[] = "Write command: ";
int pormptLen;
int main(void) {

    pormptLen = strlen(prompt);
    char buff[1024];
    int len;

    pid_t pid;
    while(true) {
        if(write(1, &prompt, pormptLen) != pormptLen) err(4, "cant write");

        if((len = read(0, &buff, sizeof(buff) - 1)) == -1) err(1, "cant read");
        if(len > 0 && buff[len -1] == '\n') buff[len - 1] = '\0';
        else buff[len] = '\0';

        if(strcmp(buff, "exit") == 0) break;

        pid = fork();
        if(pid == -1) err(2, "cant fork");

        if(pid == 0) {
            char path[1030];
            snprintf(path, sizeof(path), "/bin/%s", buff);
            execl(path, buff, (char*)NULL);
            err(3, "cant exec");
        }
        wait(NULL);
    }

    return 0;
}
