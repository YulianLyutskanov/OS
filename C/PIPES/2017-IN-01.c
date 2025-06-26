#include <err.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

    int cat_to_cut[2];
    if(pipe(cat_to_cut) == -1) err(1, "cant pipe");

    pid_t pid = fork();
    if(pid == -1) err(2, "cant fork");

    if(pid == 0) {
        close(cat_to_cut[0]);
        if(dup2(cat_to_cut[1], 1) == -1) err(4,"cant dup2");
        close(cat_to_cut[1]);

        execlp("cat", "cat", "/etc/passwd", (char*)NULL);
        err(6, "cant cat");
    }

    close(cat_to_cut[1]);
    int cut_to_sort[2];
    if(pipe(cut_to_sort) == -1) err(1, "cant pipe");

    pid = fork();
    if(pid == -1) err(2, "cant fork");

    if(pid == 0) {
        if(dup2(cat_to_cut[0], 0) == -1) err(4,"cant dup2");
        if(dup2(cut_to_sort[1], 1) == -1) err(4,"cant dup2");
        close(cut_to_sort[1]);
        close(cat_to_cut[0]);
        close(cut_to_sort[0]);

        execlp("cut", "cut", "-d", ":", "-f", "7", (char*)NULL);
        err(6, "cant cut");
    }
    close(cat_to_cut[0]);
    close(cut_to_sort[1]);
    int sort_to_uniq[2];
    if(pipe(sort_to_uniq) == -1) err(11, "cant pipe");

    pid = fork();
    if(pid == -1) err(2, "cant fork");

    if(pid == 0) {
        if(dup2(cut_to_sort[0], 0) == -1) err(4,"cant dup2");
        if(dup2(sort_to_uniq[1], 1) == -1) err(4,"cant dup2");
        close(sort_to_uniq[0]);
        close(sort_to_uniq[1]);
        close(cut_to_sort[0]);

        execlp("sort", "sort", (char*)NULL);
        err(6, "cant sort");
    }
    close(cut_to_sort[0]);
    close(sort_to_uniq[1]);

    pid = fork();
    if(pid == -1) err(2, "cant fork");
    if(pid == 0) {
        if(dup2(sort_to_uniq[0], 0) == -1) err(4,"cant dup2");
        close(sort_to_uniq[0]);

        execlp("uniq", "uniq", "-c", (char*)NULL);
        err(6, "cant uniq");
    }

    close(sort_to_uniq[0]);

    while(wait(NULL) > 0) {}

    return 0;
}
