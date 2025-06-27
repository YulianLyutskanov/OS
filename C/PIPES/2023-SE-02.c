#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

char str[] = "found it!";
int main(int argc, char* argv[]) {

    if(argc < 2) errx(26,">0 params needed");

    int p[2];
    if(pipe(p) == -1) err(26, "cant pipe");

    for(int i = 1; i < argc; i++) {
        pid_t pid = fork();

        if(pid == -1) err(26, "cant fork");

        if(pid == 0) {
            close(p[0]);
            if(dup2(p[1], 1) == -1) err(26, "cant dup2");
            close(p[1]);

            execlp(argv[i], argv[i], (char*)NULL);
            err(26, "cant exec");
        }

    }

    close(p[1]);
    char byte;
    size_t cur = 0;

    while(read(p[0], &byte, sizeof(byte)) > 0) {
        if(byte == str[cur]) cur++;
        else cur = 0;

        if(cur == 9) {
            close(p[0]);
            //if(kill(0, SIGTERM) < 0 ) err(26, "cant kill");
            exit(0);
        }
    }

    int status;
    while(wait(&status) > 0) {
        if(!WIFEXITED(status)) err(26, "some procceses aren't finished");
    }
    close(p[0]);
    return 1;
}
