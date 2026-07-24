#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/process.h"
#include "src/device.h"
#include "src/doctor.h"


/* Max supported devices: 16 (D_MAX in device.h) */
int main(int argc, char **argv) {
  int use_fake = (argc > 1 && strcmp(argv[1], "--fake") == 0);
  const char *fake_devices =
  "List of devices attached\n"
  "34031JEGR19887\tdevice\n"
  "emulator-5554\toffline\n"
  "192.168.1.10:5555\tunauthorized\n"
  "R58M12ABCDE\tsideload\n"
  "\n";
  
  // ADB DEVICES
  FILE *adb_devices = proc_open("adb devices");
  if (!adb_devices) return 1;
  size_t count = 0;
  size_t capacity = 16;
  char *device_buffer = malloc(capacity);
    
  if (!device_buffer) { proc_close(adb_devices); return 1; }
  device_buffer[0] = '\0';

  char str_devices[MAX_LEN];
   
  while (fgets(str_devices, MAX_LEN, adb_devices)) {
    device_buffer = append(device_buffer, &count, &capacity, str_devices);
    //if (!device_buffer) { proc_close(adb_devices); return 1; }
  }
  struct device d[D_MAX];
  int n_devices = 0;
  if (use_fake) {
    n_devices = compute_lines(fake_devices, d);
  } else {
    n_devices = compute_lines(device_buffer, d);
  }
   
  if(n_devices == 0) {
    printf("NO DEVICE CONNECTED\n");
    free(device_buffer);
    proc_close(adb_devices);
    return 1;
  }
  for(int i = 0; i < n_devices; i++){
    printf("Id: %s\tState: %s\n", d[i].id, d[i].state);
    diagnose(&d[i]);
  }
  //    printf("%s", device_buffer);
  free(device_buffer);
  proc_close(adb_devices); // <-- proc_close invece di pclose

  // ADB SHELL
  FILE *adb_shell_cat = proc_open("adb shell cat /proc/net/unix");
  if(!adb_shell_cat) return 1;
  count = 0;
  capacity = 16;
  char *shell_buffer = malloc(capacity);
  if(!shell_buffer) return 1;
  shell_buffer[0] = '\0';
  char str_shell[MAX_LEN];
  while (fgets(str_shell, MAX_LEN, adb_shell_cat)) {
    if(strstr(str_shell, "_devtools_remote"))
      shell_buffer = append(shell_buffer, &count, &capacity, str_shell);
  }
  char *sp = shell_buffer;
  while (*sp) {
    printf("%c", *sp);
    sp++;
  }
  free(shell_buffer);
  proc_close(adb_shell_cat);
  return 0;
}
