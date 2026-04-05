// sensordata.h - shared between kernel and user space
//
// All values are stored as integers scaled by 100 to avoid
// floating point (xv6 has no FPU ABI in user space).
//   e.g. temperature = 2350 means 23.50 °C
//        humidity    = 5500 means 55.00 %
//
// The kernel keeps exactly one instance of this struct
// (kernel_sensordata, defined in syssensor.c).  User programs
// obtain a snapshot via the getsensordata() system call.

struct sensordata {
  int temperature;  // °C  × 100  (e.g. 2350 = 23.50 °C)
  int humidity;     // %   × 100  (e.g. 5500 = 55.00 %)
  int airquality;   // AQI index  (0 = best, 500 = hazardous)
  int energyusage;  // Watts × 100 (e.g. 15050 = 150.50 W)
  int waterusage;   // L/hr × 100  (e.g.  300 =   3.00 L/hr)
};
