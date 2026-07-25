#include "device.h"
#include "process.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Runs "adb devices", captures its output and parses it into `out`.
 * `out` must have room for `max` entries. Returns the number of devices
 * found (possibly zero), or -1 if adb could not be started or memory ran out.
 */
int find_devices(struct device *out, int max) {
  FILE *adb_devices = proc_open("adb devices");
  if (!adb_devices) return -1;
  size_t count = 0;
  size_t capacity = 16;
  char *device_buffer = malloc(capacity);
    
  if (!device_buffer) { proc_close(adb_devices); return -1; }
  device_buffer[0] = '\0';

  char str_devices[MAX_LEN];
  
  while (fgets(str_devices, MAX_LEN, adb_devices)) {
    device_buffer = append(device_buffer, &count, &capacity, str_devices);
  }
  int n = compute_lines(device_buffer, out, max);
  free(device_buffer);
  proc_close(adb_devices);
  return n;
}


/* Appends `line` to the end of `buffer`, growing it when it runs out of room.
 *
 * The buffer may be moved by realloc, so the caller must always reassign the
 * returned pointer:
 *
 *     buffer = append(buffer, &count, &capacity, line);
 *
 * `count` (bytes written so far) and `capacity` (bytes allocated) are updated
 * in place. Returns NULL if the allocation fails.
 *
 * The buffer stays null terminated after every call, so it can be passed to
 * any string function at any time.
 */
char *append(char *buffer, size_t *count, size_t *capacity, const char *line) {
    const char *l = line;
    while (*l) {
        if (*count + 1 >= *capacity) {
            size_t nc = *capacity * 2;
            char *nb = realloc(buffer, nc);
            if (!nb) return NULL;
            buffer = nb;
            *capacity = nc;
        }
        buffer[*count] = *l;
        (*count)++;
        l++;
    }
    buffer[*count] = '\0';
    return buffer;
}

/* Splits a single line from "adb devices" into a device struct.
 *
 * The line looks like this, with a tab between the two fields:
 *
 *     R3CT90XXXXX<TAB>unauthorized
 *
 * Both fields are copied into the struct, so the device owns its own data and
 * stays valid after the source line is overwritten. Fields longer than the
 * struct are truncated rather than overflowing.
 */
void allocate_d(char *str, struct device *d){
  char *src = str;
  const char del = '\t';
  int count = 0;

  while(*src && *src != del && count < D_ILEN - 1){
    d->id[count] = *src;
    count++;
    src++;
  }
  d->id[count] = '\0';

  count = 0;
  if(*src == del) src++;

  while(*src && count < D_SLEN - 1){
    d->state[count] = *src;
    src++;
    count++;
  }
  d->state[count] = '\0';
}

/* Parses the output of "adb devices" into an array of devices.
 *
 * A line is treated as a device only if it contains a tab, which is what adb
 * uses to separate the serial from the state. Everything else is ignored:
 * the "List of devices attached" header, blank lines, and the daemon startup
 * messages that adb prints on first run.
 *
 * Stops after `max` devices to avoid overflowing `d`. Returns how many were
 * found, which can be zero.
 */
int compute_lines(const char *buffer, struct device *d, int max){
  char line[MAX_LEN];
  int count = 0;
  int count_d = 0;
  const char del = '\t';
  while(*buffer && count_d < max){
    if(*buffer == '\n'){
      line[count] = '\0';
      char *tab = strchr(line, del);
      if(tab){
        allocate_d(line, &d[count_d]);
        count_d++;
      }
      count = 0;
    } else if(count < MAX_LEN - 1){
      line[count] = *buffer;
      count++;
    }
    buffer++;
  }
  return count_d;
}
