#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    if(argc != 2) err(1, "2 params needed");


    pid_t pid = fork();
    if(pid == -1) err(2, "cant fork");

    if(pid == 0) {
        int fd = open("temp", O_RDONLY);
        if(fd == -1) err(3, "cant open");

        dup2(fd, 0);
        close(fd);

        execlp(argv[1], argv[1], (char*)NULL);
        err(4, "cant exec");

    }

    int status;
    wait(&status);

    if(WIFEXITED(status) && (WEXITSTATUS(status) != 0)) err(7, "process exited with non 0 status");
    else if(WIFSIGNALED(status)) err(8, "child terminated");

    unlink("temp");
    return 0;
}
