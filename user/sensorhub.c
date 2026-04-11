// sensorhub.c - user-space environmental sensor hub demo for xv6.
//
// This program launches one process per sensor, simulates a single
// sensor reading from each, then reads the latest shared snapshot
// using getsensordata().

#include "kernel/types.h"
#include "kernel/sensordata.h"
#include "kernel/alert.h"
#include "kernel/elog.h"
#include "user/user.h"

static char *sensor_name(int sensor_id)
{
  switch(sensor_id){
  case SENSOR_TEMPERATURE:  return "temperature";
  case SENSOR_AIR_QUALITY:  return "air-quality";
  case SENSOR_HUMIDITY:     return "humidity";
  case SENSOR_ENERGY_USAGE: return "energy";
  case SENSOR_WATER_USAGE:  return "water";
  default: return "unknown";
  }
}

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
  int sensors[] = {
    SENSOR_TEMPERATURE,
    SENSOR_AIR_QUALITY,
    SENSOR_HUMIDITY,
    SENSOR_ENERGY_USAGE,
    SENSOR_WATER_USAGE,
  };

  int values[] = {
    2550,  // 25.50 °C
    140,   // AQI 140
    6200,  // 62.00 %
    13250, // 132.50 W
    410,   // 4.10 L/hr
  };

  for(int i = 0; i < 5; i++){
    int pid = fork();
    if(pid < 0){
      printf("sensorhub: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      int sid = sensors[i];
      int value = values[i];
      logevent(EVENT_SENSOR_UPDATE, sid, value);
      printf("sensorhub: %s reported %d\n", sensor_name(sid), value);
      exit(0);
    }
  }

  for(int i = 0; i < 5; i++)
    wait(0);

  struct sensordata sd;
  if(getsensordata(&sd) < 0){
    printf("sensorhub: getsensordata failed\n");
    exit(1);
  }

  printf("\n=== Sensor Hub Snapshot ===\n");
  printf("Temperature  : "); print_scaled(sd.temperature); printf(" C\n");
  printf("Humidity     : "); print_scaled(sd.humidity);    printf(" %%\n");
  printf("Air Quality  : %d AQI\n", sd.airquality);
  printf("Energy Usage : "); print_scaled(sd.energyusage); printf(" W\n");
  printf("Water Usage  : "); print_scaled(sd.waterusage);  printf(" L/hr\n");
  printf("===========================\n");

  exit(0);
}
