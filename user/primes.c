#include "../kernel/types.h"
#include "user.h"

#define RD 0
#define WR 1

// 第一个进程向管道中写入所有数据
// 第二个进程依次读取数据，如果读的是第一个，则直接输出
// 如果读的不是第一个，将各个数据读出后除以上一个数据
// 有余数的再通过管道扔给下一个进程


// 从左边的管道接收数据
void prime(int lpipe[2]){
    close(lpipe[WR]);
    int p[2];
    pipe(p);
    
    int data, first;  // 存储第一个数据， 作为输出，同时用第一个数据筛后边的数
    if(read(lpipe[RD], &first, sizeof(int)) != sizeof(int)){
        return;
    }  //递归出口
    printf("prime %d\n", first);
    while(read(lpipe[RD], &data, sizeof(int)) == sizeof(int)){
        if(data % first){
            write(p[WR], &data, sizeof(int));
        }
    }
    close(lpipe[RD]);
    if(fork() == 0){
        prime(p);
    }else{
        close(p[WR]);
        wait(0);
    }

}

int main(){

    int p[2];
    pipe(p);

    //  第一个进程写入数据
    for(int i = 2; i <= 35; i ++){
        write(p[WR], &i, sizeof(int));
    }

    if(fork() == 0){
        prime(p);
    }else{
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    exit(0);
}