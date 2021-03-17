#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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

int sys_ps(void) {
  return ps();
}
