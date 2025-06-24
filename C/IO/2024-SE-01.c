#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

struct node {
    uint64_t next;
    uint32_t data[126];
};

typedef struct node node_t;

void mySeek(int fd, uint64_t offset);
void mySeek(int fd, uint64_t offset) {
    if(lseek(fd, offset, SEEK_SET) == -1) err(5, "cant seek");
}

void writeNode(int fd, uint64_t offset, node_t node);
void writeNode(int fd, uint64_t offset, node_t node) {
    mySeek(fd, offset);
    if(write(fd, &node, sizeof(node)) != sizeof(node)) err(11, "cant write");
}

void copyFile(int from, int to) {
    mySeek(from, 0);
    mySeek(to, 0);

    uint8_t buf[4096];
    int16_t bytesRead;

    while((bytesRead = read(from, &buf, sizeof(buf))) > 0) {
        if(write(to, &buf, sizeof(buf)) != bytesRead) err(14, "cant write");
    }

    if(bytesRead == -1) err(15, "cant read");
}

int main(int argc, char* argv[]) {

    if(argc != 2) errx(1, "2 args needed");

    int fd1 = open(argv[1], O_RDWR);
    if(fd1 == -1) err(2, "cant open");

    struct stat s;
    if(fstat(fd1, &s) == -1) err(3, "cant stat");
    if(s.st_size % sizeof(node_t) != 0) errx(4, "invalid file");

    char temp[] = "fileXXXXXX";
    int fd2 = mkstemp(temp);
    mySeek(fd2, s.st_size - 1);
    uint8_t null = 0x00;
    if(write(fd2, &null, sizeof(null)) !=  sizeof(null)) err(6, "cant write");

    node_t node;
    int16_t bytesRead;
    int64_t next = 0;

    while((bytesRead = read(fd1, &node, sizeof(node))) == sizeof(node)) {
        writeNode(fd2, next, node);
        next = node.next * sizeof(node_t);

        if(next == 0) break;
        if(next > s.st_size) errx(12, "invalid next");

        mySeek(fd1, next);
    }

    if(bytesRead == -1) err(9, "cant read");

    copyFile(fd2, fd1);
    close(fd1);
    close(fd2);
    execlp("rm", "rm", temp, NULL);
    err(15, "rm failed");
    return 0;
}
