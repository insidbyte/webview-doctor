#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/process.h"
#include "src/device.h"
#include "src/doctor.h"
#include "src/webview.h"



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
  
  
  struct device d[D_MAX];
  int n_devices = 0;
  if (use_fake) {
    n_devices = compute_lines(fake_devices, d, D_MAX);
  } else {
    n_devices = find_devices(d, D_MAX);
  }

  if (n_devices <= 0) {
    printf("No devices connected.\n");
    return 1;
  }
  
  for(int i = 0; i < n_devices; i++){
    printf("Id: %s\tState: %s\n", d[i].id, d[i].state);
    diagnose(&d[i]);
    struct webview wv[WV_MAX];
    int finds_wv = find_webviews(d[i].id, wv, WV_MAX);
    printf("Webview Sockets:\n");
    for (int j = 0; j < finds_wv; j++)
        printf("%s -> %s\n", wv[j].device_id, wv[j].socket);
  }
  
  return 0;
}
