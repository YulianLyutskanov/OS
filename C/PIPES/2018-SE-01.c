#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    if(argc != 2) err(1, "1 param needed");

    int find_to_sort[2];
    if(pipe(find_to_sort) == -1) err(2, "cant pipe");

    int pid = fork();
    if(pid == -1) err(3, "cant fork");

    if(pid == 0) {
        close(find_to_sort[0]);
        if(dup2(find_to_sort[1], 1) == -1) err(4, "Cant dup2");
        close(find_to_sort[1]);

        execlp("find", "find", argv[1], "-type", "f", "-printf", "%p %C@\n", (char*)NULL);
        err(5, "cant execlp");
    }

    close(find_to_sort[1]);
    int sort_to_head[2];

    if(pipe(sort_to_head) == -1) err(2, "cant pipe");
    if((pid = fork()) == -1) err(3, "cant fork");

    if(pid == 0) {
        if(dup2(find_to_sort[0], 0) == -1) err(4, "Cant dup2");
        if(dup2(sort_to_head[1], 1) == -1) err(4, "Cant dup2");
        close(find_to_sort[0]);
        close(sort_to_head[1]);

        execlp("sort", "sort", "-r", "-n", "-t", " ", "-k", "2", (char*)NULL);
        err(5, "cant execlp");

    }

    close(find_to_sort[0]);
    close(sort_to_head[1]);
    int head_to_cut[2];

    if(pipe(head_to_cut) == -1) err(2, "cant pipe");
    if((pid = fork()) == -1) err(3, "cant fork");

    if(pid == 0) {
        if(dup2(sort_to_head[0], 0) == -1) err(4, "Cant dup2");
        if(dup2(head_to_cut[1], 1) == -1) err(4, "Cant dup2");
        close(sort_to_head[0]);
        close(head_to_cut[1]);

        execlp("head", "head", "-n", "1", (char*)NULL);
        err(5, "cant execlp");
    }

    close(sort_to_head[0]);
    close(head_to_cut[1]);

    if((pid = fork()) == -1) err(3, "cant fork");

    if(pid == 0) {
        if(dup2(head_to_cut[0], 0) == -1) err(4, "Cant dup2");
        close(head_to_cut[0]);

        execlp("cut", "cut", "-d", " ", "-f", "1", (char*)NULL);
        err(5, "cant execlp");
    }

    close(head_to_cut[0]);
    while(wait(NULL) > 0) { }
    return 0;
}
