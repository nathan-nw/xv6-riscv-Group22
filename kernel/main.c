#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "elog.h"
#include "alert.h"
#include "sensordata.h"
extern struct semaphore global_sem;

struct sensordata latest_sensordata;

volatile static int started = 0;
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    eloginit();      // initialize logging system
    alertinit();     // initialize alert system
    latest_sensordata.temperature = 0;
    latest_sensordata.humidity = 0;
    latest_sensordata.airquality = 0;
    latest_sensordata.energyusage = 0;
    latest_sensordata.waterusage = 0;
    userinit();      // first user process
    __sync_synchronize();
    sem_init(&global_sem, 1);
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();
}