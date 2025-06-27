#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

bool isDigit(char ch);
bool isDigit(char ch) {
    return ch >= '1' && ch <= '9';
}

char ding[] = "DING ";
char dong[] = "DONG\n";
char mess = '\0';

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params expected");
    if(strlen(argv[1]) != 1 || strlen(argv[2]) != 1) errx(2, "one digit numbers needed");
    if(!isDigit(argv[1][0]) || !isDigit(argv[2][0])) errx(3, "both must be digits > 0");

    uint8_t n = argv[1][0] - '0';
    uint8_t d = argv[2][0] - '0';

    int parent_to_child[2];
    int child_to_parent[2];
    if(pipe(parent_to_child) == -1) err(4, "cant pipe");
    if(pipe(child_to_parent) == -1) err(4, "cant pipe");

    pid_t pid = fork();
    if(pid == -1) err(6, "cant fork");

    if(pid == 0) { //child
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        for(uint8_t i = 0; i < n; i++) {
            char rd;
            if(read(parent_to_child[0], &rd, sizeof(rd)) != sizeof(rd)) err(9, "cant read");
            if(write(1, &dong, sizeof(dong)) != sizeof(dong)) err(16, "cant write");
            if(write(child_to_parent[1], &mess, sizeof(mess)) != sizeof(mess)) err(11, "cant write");
        }
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        return 0;
    }

    close(parent_to_child[0]);
    close(child_to_parent[1]);

    char rd;
    for(uint8_t i = 0; i < n; i++) {
        if(i != 0) {
            if(read(child_to_parent[0], &rd, sizeof(rd)) != sizeof(rd)) err(9, "cant read");
            sleep(d);
        }

        if(write(1, &ding, sizeof(ding)) != sizeof(ding)) err(7, "cant write");
        if(write(parent_to_child[1], &mess, sizeof(mess)) != sizeof(mess)) err(7,"cant write");

    }

    sleep(d);
    close(parent_to_child[1]);
    close(child_to_parent[0]);
    wait(NULL);
    return 0;
}
