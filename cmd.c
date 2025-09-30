#include "cmd.h"

Cmd cmd = {0};

int main(int argc, char** argv) {
    cmd_append(&cmd, "cc -ggdb -o cmd cmd.c");
    cmd_print(cmd);
    cmd_run(cmd);
    cmd_reset(&cmd);
    return 0;
}
