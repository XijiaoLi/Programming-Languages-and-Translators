#pragma once

#include <tuple>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

inline int get_pid() {
    return getpid();
}

/*
 * Run a program and wait to finish. Use current stdin, stdout, stderr.
 * All of the parameters should be const char *
 * Return: <pid, return_code>. If fails, pid will be 0.
 */
template<typename... Args>
std::tuple<int, int> run_and_wait(const char *file, Args&&... args) noexcept {
    pid_t pid = 0;
    int status = 0;
    if ((pid = fork()) < 0) {
        return std::make_tuple(0, 0);
    } else if (pid == 0) {
        execlp(file, file, args..., (const char *)0);
        _exit(127);
    }

    if ((pid = waitpid(pid, &status, 0)) < 0) {
        return std::make_tuple(0, 0);
    }

    if (WIFEXITED(status)) {
        return std::make_tuple((int)pid, WEXITSTATUS(status));
    }

    return std::make_tuple(0, 0);
}