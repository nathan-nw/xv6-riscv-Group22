#include "kernel/types.h"
#include "user/user.h"

void critical(int id) {
  printf("Process %d ENTER\n", id);
  for (volatile int i = 0; i < 100000000; i++);
  printf("Process %d EXIT\n", id);
}

int main() {
  sem_init(1);   

  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      sem_wait();
      critical(i);
      sem_signal();
      exit(0);
    }
  }

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  exit(0);
}

/*I implemented a semaphore in xv6 to control 
access to a critical section. I added new system 
calls for sem_init, sem_wait, and sem_signal, 
and connected them through syscall.h, syscall.c, 
sysproc.c, and user space. Then I wrote a user 
program (semtest) that forks multiple processes. 
Each process tries to enter a critical section, 
but the semaphore ensures only one enters at a time. 
This demonstrates proper synchronization and 
prevents race conditions.*/
