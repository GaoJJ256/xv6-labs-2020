#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
#include "../kernel/fs.h"

#define MAXARGV 20
#define MAXCNT 30

void famtinput(char * buf, char * arg){
    int idx = -1;
    while(idx < MAXCNT){
        if(buf[++ idx] == '\n') break;
        arg[idx] = buf[idx];
    }
    arg[idx] = 0;
}


int main(int argc, char *argv[]){
    // argv[1] 是所要执行的命令
    char * oplist[MAXCNT];
    char cmd[MAXARGV];

    if(argc == 1){
        strcpy(cmd, "echo");
    }else{
        strcpy(cmd, argv[1]);
    }
    
    char buf[MAXARGV];

    int arg_num = 0;
    while(1){
        memset(buf, 0, sizeof(buf));
        gets(buf, MAXARGV);
        if(strlen(buf) == 0 || strlen(buf) > MAXARGV)
            break;
        char * arg = (char *)malloc(sizeof(MAXARGV));
        famtinput(buf, arg);
        oplist[arg_num ++] = arg;
    }

    char * argv2exec[MAXCNT];
    argv2exec[0] = cmd;
    int idx = 1;
    for(; idx < argc - 1; idx ++)
        argv2exec[idx] = argv[idx + 1];
    int t = idx;
    for(; idx - t < arg_num; idx ++)
        argv2exec[idx] = oplist[idx - t];

    // printf("-----\n");
    // for(int i = 0; i < idx; i ++) printf("%s\n", argv2exec[i]);
    // printf("%s\n", cmd);
    // printf("-----\n");

    int pid;
    if((pid = fork()) == 0){
        exec(cmd, argv2exec);
    }else{
        wait(0);
    }
    exit(0);
}