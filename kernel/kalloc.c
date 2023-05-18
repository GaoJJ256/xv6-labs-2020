// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel. 指向内核映像的末尾地址
                   // defined by kernel.ld.

// 空闲链表
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;  // 自旋锁
  struct run *freelist;  // 指向可用分配空间的头节点 
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,  释放pa指向的物理内容
// which normally should have been returned by a   
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
// 函数用于将一个物理内存页面释放，它通常应该是通过调用 kalloc() 函数分配的。
// 该函数先检查要释放的内存页面地址是否合法，然后使用 memset() 函数将页面填充为垃圾值，
// 以便检测未释放的引用。最后，将该页面添加到空闲链表的开头。
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory. 分配一个4096B的物理页面
// Returns a pointer that the kernel can use.  返回一个内核可用的指针
// Returns 0 if the memory cannot be allocated.  如果无法分配的话返回0
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

// sysinfo实验内容
uint64 
acq_mem(void)
{
    struct run *r;
    uint64 cnt = 0;
    acquire(&kmem.lock);
    r = kmem.freelist;  // 空闲链表
    while(r){
        r = r->next;
        cnt ++;
    }
    release(&kmem.lock);

  return cnt * PGSIZE;
}