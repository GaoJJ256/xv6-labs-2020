#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


// 返回当前路径的最后一个文件
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;  
  struct dirent de;  // 目录信息
// struct dirent {
//   ushort inum;
//   char name[DIRSIZ];
// };
  struct stat st;  // 文件描述块

  if((fd = open(path, 0)) < 0){  //获得文件描述符
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){  
    // fstat 通过文件描述符读取相关的stat信息，并放入st中
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path); 
    p = buf+strlen(buf); 
    *p++ = '/';  // 给路径尾部添上‘/’
    while(read(fd, &de, sizeof(de)) == sizeof(de)){  // 循环读取目录中的文件
        // de为dirent类型
      if(de.inum == 0)  // 文件不存在或者已经被删除
        continue;
      memmove(p, de.name, DIRSIZ);  //将文件路径末尾添上文件名称
      p[DIRSIZ] = 0; // 为结尾添加一个空字符，让buf成为一个标准的字符串
      if(stat(buf, &st) < 0){  // 获取以buf为路径的文件信息
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
