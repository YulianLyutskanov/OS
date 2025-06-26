#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    if(argc != 2) errx(1, "2 params needed");

    if(mkfifo("temp", 0666) == -1) err(2, "cant mkfifo");

    int fd = open("temp", O_WRONLY);
    if(fd == -1) err(3, "cant open");

    dup2(fd, 1);

    execlp("cat", "cat", argv[1], (char*)NULL);
    err(7, "cant exec");

    return 0;
}
