// syscalls.c
#include <unistd.h>  // 有些编译器会在这里定义 weak symbol 原型

int _kill(int pid, int sig);
int _getpid(void);
void _exit(int status);
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    return -1;
}

int _getpid(void)
{
    return 1;
}

void _exit(int status)
{
    (void)status;
    while (1);
}
