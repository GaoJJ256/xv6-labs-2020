#include "../kernel/types.h"
#include "user.h"

#define BUFF_SIZE 1
#define RD 0 // 读端
#define WR 1 // 写端
int p_pc[2]; // 父进程->子进程
int p_cp[2]; // 子进程->父进程

int main(int argc, char *argv[])
{
    pipe(p_cp);
    pipe(p_pc);

    int pid = fork();

    if (pid < 0)
    {
        fprintf(2, "fork reeor!\n");
        // 关闭管道
        close(p_cp[RD]);
        close(p_cp[WR]);
        close(p_pc[RD]);
        close(p_pc[WR]);
        exit(1);
    }
    char buf = '_'; // "乒乓" & 缓冲区
    int exit_status = 0;
    if (pid == 0)
    { // 子进程
        // 管道读端没有close时，管道写端会被阻塞
        close(p_pc[WR]);
        close(p_cp[RD]);
        if (read(p_pc[RD], &buf, sizeof(char)) != sizeof(char))
        {
            exit_status = 1;
            fprintf(2, "child read error\n");
        }
        else
        {
            // 子进程读取成功
            fprintf(1, "%d: received ping\n", getpid());
        }
        close(p_pc[WR]);
        if (write(p_cp[WR], &buf, sizeof(char)) != sizeof(char))
        {
            exit_status = 1;
            fprintf(2, "child wirte error\n");
        }
        close(p_cp[WR]);
        exit(exit_status);
    }
    else
    {
        close(p_pc[RD]);
        close(p_cp[WR]);
        if (write(p_pc[WR], &buf, sizeof(char)) != sizeof(char))
        {
            exit_status = 1;
            fprintf(2, "parent write error\n");
        }
        close(p_pc[WR]);

        if (read(p_cp[RD], &buf, sizeof(char)) != sizeof(char))
        {
            exit_status = 1;
            fprintf(2, "parent read error\n");
        }
        else
        {
            fprintf(1, "%d: received pong\n", getpid());
        }
        close(p_cp[RD]);
        exit(exit_status);
    }
    exit(exit_status);
}