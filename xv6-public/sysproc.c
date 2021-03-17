#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//Custom defined system calls start here
int
sys_print_count(void) {
  for(int i=0; i<100; ++i){
    int index = printOrder[i];
    if(count_syscall[index] > 0){
      cprintf("%s %d\n", syscallnames[index], count_syscall[index]);
    }
  }
  return 0;
}

int
sys_toggle(void) {
  trace_syscall = 1-trace_syscall; //change 0 to 1, and 1 to 0 where 0: trace off, 1: trace on
  if(trace_syscall == 0){
    for(int i=0; i<100; ++i){
      count_syscall[i] = 0;
    }
  }
  return 0;
}

int
sys_add(void) {
  int a, b;
  if(argint(0, &a) < 0) return -1;
  if(argint(1, &b) < 0) return -1;
  return a+b;
}

int
sys_ps(void) {
  return ps();
}

//Data structure for sys_send and sys_recv
struct Node {
  int sender_pid;
  void *msg;
};

#define MAXM 100

struct {
  struct spinlock lock;
  struct Node q[NPROC][MAXM]; //Max of MAXM messages per process
  int head[NPROC], tail[NPROC];
} mQueue;

void qinit(void) {
  initlock(&mQueue.lock, "mQueue");
  for(int i=0;i<NPROC;++i){
    mQueue.head[i] = mQueue.tail[i] = -1;
  }
}

int
sys_send(void) {
  int sender_pid, rec_pid;
  void *msg;
  if(argint(0, &sender_pid) < 0) return -1;
  if(argint(1, &rec_pid) < 0) return -1;
  if(argstr(2, (char**)&msg) < 0) return -1;
  if(rec_pid < 0 || rec_pid >= NPROC) return -1;

  acquire(&mQueue.lock); //Acquire lock on message queue

  //Add the message to queue for the process with pid rec_pid
  if(mQueue.tail[rec_pid] == -1){ // Queue empty, insert first element
    mQueue.head[rec_pid] = mQueue.tail[rec_pid] = 0;
  } else if((mQueue.tail[rec_pid]+1)%MAXM == mQueue.head[rec_pid]){ //Queue full
    release(&mQueue.lock); //Lock must be released before exiting
    return -1; //Indicate error as memory full
  } else {
    mQueue.tail[rec_pid] = (mQueue.tail[rec_pid]+1)%MAXM; //Insert new element
  }
  //Add the message to queue at tail
  mQueue.q[rec_pid][mQueue.tail[rec_pid]].sender_pid = sender_pid;
  mQueue.q[rec_pid][mQueue.tail[rec_pid]].msg = msg;

  release(&mQueue.lock); //Release the lock
  return 0;
}

int
sys_recv(void){
  //TODO
  return 0;
}
