// syssensor.c - implementation of the getsensordata() system call.
//
// OS concept rationale
// --------------------
// User programs must never read kernel memory directly. getsensordata()
// builds a snapshot of the latest sensor readings by scanning the shared
// event log (elog) maintained by the teammate's logevent() syscall, then
// uses copyout() to transfer it safely into user space.
//
// Design: elogread() returns all log entries oldest→newest. We walk them
// in order, updating the per-sensor value each time we see an
// EVENT_SENSOR_UPDATE for that sensor ID. The last matching entry for
// each sensor is therefore the most recent reading. Sensors not yet seen
// in the log fall back to the compile-time defaults in kernel_sensordata.

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sensordata.h"
#include "elog.h"

// ---------------------------------------------------------------------------
// Default / boot values (used when no elog entry exists yet for a sensor).
// Units: see sensordata.h  (all values × 100).
// ---------------------------------------------------------------------------
struct sensordata kernel_sensordata = {
  .temperature = 2200,   // 22.00 °C
  .humidity    = 4500,   // 45.00 %
  .airquality  = 50,     // AQI 50
  .energyusage = 15000,  // 150.00 W
  .waterusage  =   300,  //  3.00 L/hr
};

// ---------------------------------------------------------------------------
// sys_getsensordata()
// ---------------------------------------------------------------------------
// Builds a live snapshot by replaying the most recent EVENT_SENSOR_UPDATE
// entry from the elog for each sensor, then copies it to user space.
//
// Returns 0 on success, -1 on bad user pointer.
uint64
sys_getsensordata(void)
{
  uint64 uaddr;
  struct sensordata snap;
  struct elog_entry buf[ELOG_SIZE];
  int n, i;

  argaddr(0, &uaddr);
  if(uaddr == 0)
    return -1;

  // Start from the compiled-in defaults so any sensor that has no log
  // entry yet still returns a sensible value.
  snap = kernel_sensordata;

  // Read all log entries (oldest → newest) into a local kernel buffer.
  // elogread() holds the elog spinlock internally, so this is safe to
  // call from a syscall handler.
  n = elogread(buf, ELOG_SIZE);

  // Walk entries in order. For EVENT_SENSOR_UPDATE, overwrite the
  // matching field in snap. Later entries overwrite earlier ones, so
  // snap ends up holding the most recent value for every sensor.
  for(i = 0; i < n; i++){
    if(buf[i].event_type != EVENT_SENSOR_UPDATE)
      continue;
    switch(buf[i].sensor_id){
    case SENSOR_TEMPERATURE:  snap.temperature = buf[i].value; break;
    case SENSOR_HUMIDITY:     snap.humidity    = buf[i].value; break;
    case SENSOR_AIR_QUALITY:  snap.airquality  = buf[i].value; break;
    case SENSOR_ENERGY_USAGE: snap.energyusage = buf[i].value; break;
    case SENSOR_WATER_USAGE:  snap.waterusage  = buf[i].value; break;
    }
  }

  // copyout() validates uaddr against the user page table before writing.
  if(copyout(myproc()->pagetable, uaddr,
             (char *)&snap, sizeof(snap)) < 0)
    return -1;

  return 0;
}
