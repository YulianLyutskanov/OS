#include <err.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>

int main(int argc, char* argv[]) {

    if(argc < 3) err(1, "2 params needed");

    if(strlen(argv[1]) != 1) errx(2, "one digit needed");
    uint8_t num = argv[1][0] -'0';
    if(num == 0 || num > 9) errx(3, "should be digit");


    pid_t pid;
    int fd = open("run.log", O_TRUNC | O_CREAT | O_WRONLY, 0666);
    if(fd == -1) err(7, "cant open");

    char buf[128];
    int status;
    int lastStatus = INT_MAX;
    int lastDiff = INT_MAX;

    while(true) {
        time_t start = time(NULL);
        pid = fork();
        if(pid == -1) err(4, "cant fork");
        if(pid == 0) {
            if(argc == 3) execlp(argv[2],argv[2],(char*)NULL);
            else execvp(argv[2], &argv[2]);
            err(6, "cant exec");
        }

        time_t end = time(NULL);
        waitpid(pid, &status, 0);

        if(WIFSIGNALED(status)) status = 129;
        else if(WIFEXITED(status)) status = WEXITSTATUS(status);
        else status = 0;

        snprintf(buf, sizeof(buf), "%ld %ld %d\n", start, end, status);

        if(write(fd, buf, strlen(buf)) != (long int)strlen(buf)) err(8, "cant write");
        if(status != 0 && lastStatus != 0 && lastDiff < num && (end - start) < num) {
            break;
        }
        lastStatus = status;
        lastDiff = end - start;
    }
    close(fd);
    return 0;
}
