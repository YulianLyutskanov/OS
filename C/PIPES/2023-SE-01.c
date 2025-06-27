#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <string.h>

void proccessFile(char* file);
void proccessFile(char* file) {

    int p[2];
    if(pipe(p) == -1) err(12, "cant pipe");

    pid_t pid = fork();
    if(pid == -1) err(13, "cant fork");

    if(pid == 0) {
        close(p[0]);
        if(dup2(p[1], 1) == -1) err(1, "Cant dup2");

        execlp("md5sum", "md5sum", file, (char*)NULL);
        err(15, "cant exec");
    }

    close(p[1]);

    char hashName[4096];
    strcpy(hashName, file);
    strcat(hashName, ".hash");

    int fd = open(hashName, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd == -1) err(16, "cannt open");

    ssize_t readBytes;
    char byte;

    while((readBytes = read(p[0], &byte, sizeof(byte))) == sizeof(byte)) {
        if(write(fd, &byte, sizeof(byte)) != sizeof(byte)) err(18, "cant write");
    }

    if(readBytes == -1) err(17, "cant read");
    byte = '\n';
    if(write(fd, &byte, sizeof(byte)) != sizeof(byte)) err(19, "cant write");

    close(p[0]);
    wait(NULL);

    close(fd);
}

int main(int argc, char* argv[]) {

    if(argc != 2) errx(1, "1 param needed");

    int findP[2];
    if(pipe(findP) == -1) err(2, "cant pipe");

    pid_t pid = fork();
    if(pid == -1) err(3, "cant fork");

    if(pid == 0) {
        close(findP[0]);
        if(dup2(findP[1], 1) == -1) err(4,"cant dup2");
        close(findP[1]);

        execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", "-print0", (char*)NULL);
        err(5, "cant exec");
    }
    close(findP[1]);

    ssize_t readSize;
    char byte;
    char buffer[4096];
    size_t bufferIndex = 0;
    size_t workersCount = 0;

    while((readSize = read(findP[0], &byte, sizeof(byte))) == sizeof(byte)) {
        if(bufferIndex >= 4096) errx(6, "too big name");

        buffer[bufferIndex++] = byte;

        if(byte == '\0') {
            bufferIndex = 0;

            pid_t pidWorker = fork();
            if(pidWorker == -1) err(8, "cant fork");

            if(pidWorker == 0) {

                close(findP[0]);
                proccessFile(buffer);
                return 0;
            }
            workersCount++;
        }
    }

    if(readSize == -1) err(9, "cant read");

    close(findP[0]);

    for(size_t i = 0; i < workersCount + 1; i++) {
        int status;
        wait(&status);

        if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) != 0) warnx( "child exited with sc != 0");

        } else {
            warnx("child was killed");
        }

    }

    return 0;
}
