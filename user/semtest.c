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
