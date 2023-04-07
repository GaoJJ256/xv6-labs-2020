#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
#include "../kernel/fs.h"

char * famtname(char * path){
    static char buf[DIRSIZ + 1];
    char *p;

    for(p = path + strlen(path); p >= path && * p != '/'; p --)
        ;
    p ++;

    if(strlen(p) >= DIRSIZ) return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void find(char * path, const char *target){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return ;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return ;
    }
    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            if(strcmp(famtname(path), target) == 0){
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path tpp long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p ++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                // 遇到 . .. 目录，跳过
                if(strcmp(de.name, ".") == 0) continue ;
                if(strcmp(de.name,  "..") == 0) continue ;
                // 文件不存在或者被删除，跳过
                if(de.inum == 0) continue;

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                // 如果是目录， 就递归find
                if(st.type == 1)
                    find(buf, target);
                // 如果是文件，如果名称匹配，就输出
                else if(strcmp(de.name, target) == 0)
                    printf("%s\n", buf);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        fprintf(2, "find <target dir> <terget filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);

    exit(0);
}