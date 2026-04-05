// sensortest.c - user-space test program for the getsensordata() syscall.
//
// Compile & run inside xv6:
//   $ sensortest
//
// The program calls getsensordata(), which copies the latest sensor
// readings from kernel space into our local struct, then prints them.

#include "kernel/types.h"
#include "kernel/sensordata.h"  // struct sensordata definition
#include "user/user.h"

// Helper: print an integer that is scaled by 100 as "whole.frac"
// e.g. print_scaled(2350) prints "23.50"
// xv6's printf does not support %02d, so we pad manually.
static void
print_scaled(int val)
{
  int neg = 0;
  int frac;
  if(val < 0){ neg = 1; val = -val; }
  frac = val % 100;
  if(neg) printf("-");
  if(frac < 10)
    printf("%d.0%d", val / 100, frac);
  else
    printf("%d.%d", val / 100, frac);
}

int
main(void)
{
  struct sensordata sd;

  // getsensordata() is the new system call.
  // It copies kernel_sensordata → sd using copyout(), so this program
  // never touches kernel memory directly – the boundary is enforced by
  // the MMU / page-table walk inside copyout().
  if(getsensordata(&sd) < 0){
    fprintf(2, "sensortest: getsensordata() failed\n");
    exit(1);
  }

  printf("=== Environmental Sensor Hub ===\n");
  printf("Temperature  : ");  print_scaled(sd.temperature); printf(" C\n");
  printf("Humidity     : ");  print_scaled(sd.humidity);    printf(" %%\n");
  printf("Air Quality  : %d AQI\n", sd.airquality);
  printf("Energy Usage : ");  print_scaled(sd.energyusage); printf(" W\n");
  printf("Water Usage  : ");  print_scaled(sd.waterusage);  printf(" L/hr\n");
  printf("================================\n");

  exit(0);
}
