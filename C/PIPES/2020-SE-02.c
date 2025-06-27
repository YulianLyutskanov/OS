#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdint.h>

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");

    int pfd[2];
    if(pipe(pfd) == -1) err(3, "cant pipe");

    pid_t pid = fork();
    if(pid == -1) err(2, "cant fork");

    if(pid == 0) {
        close(pfd[0]);
        if(dup2(pfd[1], 1) == -1) err(8, "cant dup");
        close(pfd[1]);

        execlp("cat", "cat", argv[1], (char*)NULL);
        err(3, "cant exec");
    }

    close(pfd[1]);

    int status;
    if(wait(&status) == -1) err(4,"cant wait");
    if(!WIFEXITED(status)) err(5, "proc was killed");
    else if(WEXITSTATUS(status) != 0) err(6, "error by cat");

    int fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd == -1) err(9, "cant open file");
    uint8_t byte;
    int8_t readBytes;

    while((readBytes = read(pfd[0], &byte, sizeof(byte))) == sizeof(byte)) {
        if(byte == 0x55) continue;
        if(byte == 0x7D) {
            if((readBytes = read(pfd[0], &byte, sizeof(byte))) == -1) err(11, "cant read");
            else if(readBytes == 0) errx(12, "invalid file");

            if(byte != (0x00 ^ 0x20) && byte != (0xFF ^ 0x20) && byte != (0x55 ^ 0x20) && byte != (0x7D ^ 0x20)) errx(13, "invalid file");

            byte ^= 0x20;
        }

        if(write(fd, &byte, sizeof(byte)) != sizeof(byte)) err(13, "cant write");
    }

    if(readBytes == -1) err(10, "cant read");
    close(pfd[0]);
    close(fd);
    return 0;
}
