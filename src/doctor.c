#include "doctor.h"
#include <string.h>
#include <stdio.h>

/* Turns an adb device state into something the user can act on.
 *
 * adb reports one word and stops there, which leaves people searching the web
 * for what "unauthorized" or "offline" actually mean. This prints the reason
 * and the next step instead.
 *
 * Output is plain ASCII on purpose. Unicode symbols look fine on Linux but
 * come out as garbage in the default Windows console.
 */
void diagnose(const struct device *d) {
  if (strcmp(d->state, "device") == 0) {
    printf("[OK] %s is ready\n", d->id);
    return;
  }

  if (strcmp(d->state, "unauthorized") == 0) {
    printf("[!] %s is not authorized\n", d->id);
    printf("    The phone is connected but USB debugging has not been allowed.\n");
    printf("    -> Check your phone screen and tap \"Allow\" on the prompt.\n");
    return;
  }

  if (strcmp(d->state, "offline") == 0) {
    printf("[!] %s is offline\n", d->id);
    printf("    The device is visible but not answering commands.\n");
    printf("    -> Unplug and replug the cable, or run 'adb kill-server'.\n");
    return;
  }

  /* Anything else: adb has states we do not handle yet (sideload, recovery,
   * bootloader). Showing the raw value is more useful than staying silent.
   */
  printf("[?] %s is in an unhandled state: %s\n", d->id, d->state);
}
