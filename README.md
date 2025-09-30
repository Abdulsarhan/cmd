# cmd

Execute shell commands using by calling C functions.
## Basic Usage

```c
#include "cmd.h"

Cmd cmd = {0};

int main(int argc, char** argv) {
    cmd_append(&cmd, "cc -ggdb -o cmd cmd.c"); // Append commands to the command string.
    cmd_print(cmd); // Prints the current command string.
    cmd_run(cmd); // Execute the shell command.
    cmd_reset(&cmd); // reset if you want to use the cmd structure again.
    return 0;
}
```
