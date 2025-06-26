#include <err.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    if(argc != 2) errx(1, "One parameter needed");

    int pfd[2];
    if(pipe(pfd) == -1) err(3, "cant pipe");

    pid_t pid = fork();

    if(pid == -1) err(2, "cant for");

    if(pid == 0) { //child
        close(pfd[0]);
        if(dup2(pfd[1], 1) == -1) err(5, "dup2");
        close(pfd[1]);

        execlp("cat", "cat", argv[1], NULL);
        err(4, "cant cat");
    }

    close(pfd[1]);
    pid = fork();
    if(pid == -1) err(8, "cant fork");

    if(pid == 0) {
        if(dup2(pfd[0], 0) == -1) err(6, "dup2");
        close(pfd[0]);

        execlp("sort", "sort", NULL);
        err(7, "cant sort");
    }

    close(pfd[0]);
    while(wait(NULL) > 0) { }

    return 0;
}
