#include <err.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

char com[] = "echo";
int main(int argc, char* argv[]) {

    if(argc > 2) errx(1, " max 1 param");
    if(argc == 2 && strlen(argv[1]) > 4) errx(2, "max lenght 4");
    if(argc == 2) strcpy(com, argv[1]);


    char arg[2][5] = { "", "" };
    char ch;
    int8_t bytesRead;
    int8_t len = 0;
    int8_t which = 0;

    while((bytesRead = read(0, &ch, sizeof(ch))) == sizeof(ch)) {
        if(ch == ' ' || ch == '\n') {
            if(len == 0) errx(5, "invalid stdin");
            arg[which][len] = '\0';
            which++;
            len = 0;
            if(which == 2) {
                pid_t pid = fork();
                if(pid == -1) err(7, "cant fork");
                if(pid == 0) {
                    execlp(com, com, arg[0], arg[1], (char*)NULL);
                    err(8, "cant execute");
                }
                wait(NULL);
                which = 0;
            }
        } else {
            if(len == 4) errx(4, "invalid stdin");
            arg[which][len] = ch;
            len++;
        }

    }

    if(bytesRead == -1) err(3, "cant read");
    if(which == 1 && len == 0) {
        execlp(com, com, arg[0], (char*)NULL);
        err(9, "cant exec");
    }
    return 0;
}
