#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void critical(int id) {
  printf("Process %d ENTER\n", id);
  sleep(50);
  printf("Process %d EXIT\n", id);
}

int main() {
  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      semwait();
      critical(i);
      semsignal();
      exit(0);
    }
  }

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  exit(0);
}
