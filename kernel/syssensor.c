// syssensor.c - implementation of the getsensordata() system call.
//
// OS concept rationale
// --------------------
// Sensor hardware (or the sensor-update processes written by other
// teammates) lives entirely in kernel space.  User programs must
// NEVER directly dereference kernel pointers – doing so would allow
// arbitrary kernel memory reads.  Instead they call getsensordata(),
// which uses copyout() to safely transfer a *snapshot* of the data
// into the user process's own virtual address space.
//
// copyout(pagetable, dst_uva, src_kva, len)
//   - walks the user page table to validate 'dst_uva'
//   - copies 'len' bytes from kernel address 'src_kva'
//   - returns -1 if 'dst_uva' is invalid or out-of-bounds
// This guarantees the kernel never writes outside the user buffer.

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sensordata.h"

// ---------------------------------------------------------------------------
// Global kernel-space sensor state
// ---------------------------------------------------------------------------
// Sensor-update processes (owned by other teammates) write here directly.
// Only one copy exists in the kernel; user programs receive a snapshot.
//
// Default / boot values are set below.  Units: see sensordata.h.
struct sensordata kernel_sensordata = {
  .temperature = 2200,   // 22.00 °C  – typical indoor temperature
  .humidity    = 4500,   // 45.00 %   – comfortable humidity
  .airquality  = 50,     // AQI 50    – good air quality
  .energyusage = 15000,  // 150.00 W  – moderate load
  .waterusage  =   300,  //   3.00 L/hr
};

// ---------------------------------------------------------------------------
// sys_getsensordata()
// ---------------------------------------------------------------------------
// System-call handler.  Receives one argument from the user:
//   a0 = pointer to a user-space struct sensordata (destination buffer)
//
// Returns:
//   0  on success  (struct sensordata filled in user space)
//  -1  on failure  (null pointer, bad address, or address outside process)
uint64
sys_getsensordata(void)
{
  uint64 uaddr;          // user-space virtual address of destination buffer

  // argaddr() reads register a0 from the saved trapframe –
  // this is the pointer the user passed as the first argument.
  argaddr(0, &uaddr);

  // Reject null pointers explicitly for a clear error code.
  if(uaddr == 0)
    return -1;

  // copyout() validates 'uaddr' against the calling process's page table
  // and performs the kernel→user copy atomically from the kernel's view.
  // If the address is invalid or the region is read-only, it returns -1.
  if(copyout(myproc()->pagetable, uaddr,
             (char *)&kernel_sensordata,
             sizeof(kernel_sensordata)) < 0)
    return -1;

  return 0;
}
