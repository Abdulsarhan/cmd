#ifndef CMD_H
#define CMD_H

#include <stdio.h>
#include <stdint.h>

typedef struct Cmd {
    char str[1024 * 1024]; // 1 Megabyte
    size_t len; // This is only used by cmd_append
}Cmd;

void cmd_append(Cmd* cmd, const char* str);
void cmd_print(Cmd cmd);
int cmd_run(Cmd cmd);
void cmd_reset(Cmd* cmd);

// Implementation
void cmd_reset(Cmd* cmd) {
    cmd->str[0] = '\0';
    cmd->len = 0;
}

size_t util_strlen(const char* str) {
    size_t strlen = 0;
    while(str[strlen] != '\0') {
        strlen++;
    }
    return strlen;
}

void cmd_append(Cmd* cmd, const char* str) {
    size_t str_len = util_strlen(str);
    for(int i = 0; i < str_len; i++) {
        cmd->str[cmd->len + i] = str[i];
    }
    cmd->len += str_len;
    cmd->str[str_len] = '\0';
}

#ifdef __unix__

#include <unistd.h>
#include <sys/wait.h>

void util_print(const char* str) {
    char new_line = '\n';
    write(STDOUT_FILENO, str, util_strlen(str));
    write(STDOUT_FILENO, &new_line, 1);
}

void cmd_print(Cmd cmd) {
    char new_line = '\n';
    write(STDOUT_FILENO, cmd.str, cmd.len);
    write(STDOUT_FILENO, &new_line, 1);
}

int cmd_run(Cmd cmd) {
    pid_t pid = fork(); // at this point, we have two processes.

    if(pid < 0) {
        util_print("fork failed!\n");
        return -1;
    }

    if(pid == 0) { // we're in the child. pause.
        execl("/bin/sh", "sh", "-c", cmd.str, (char *) NULL);
        util_print("child reached unreachable part. execl failed.");
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    return 0;
}

#elifdef _WIN32
#include <windows.h>

void util_print(const char* str) {
    DWORD written;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    char new_line = '\n';
    WriteFile(hStdout, str, (DWORD)util_strlen(str), &written, NULL);
    WriteFile(hStdout, &new_line, 1, &written, NULL);
}

void cmd_print(Cmd cmd) {
    DWORD written;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(hStdout, cmd.str, (DWORD)cmd.len, &written, NULL);
    WriteFile(hStdout, "\n", 1, &written, NULL);
}

int cmd_run(Cmd cmd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Build command string: "cmd.exe /C " + cmd.str
    char fullCmd[1024 * 2];
    fullCmd[0] = '\0';
    util_strcat(fullCmd, "cmd.exe /C ");
    util_strcat(fullCmd, cmd.str);

    if (!CreateProcessA(
            NULL,        // application name
            fullCmd,     // command line
            NULL,        // process security
            NULL,        // thread security
            FALSE,       // inherit handles
            0,           // creation flags
            NULL,        // environment
            NULL,        // current directory
            &si,
            &pi)) {
        util_print("CreateProcess failed!");
        return -1;
    }

    // Wait until child process exits
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Clean up handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
#endif // _WIN32

#endif // CMD_H
