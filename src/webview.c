#include "webview.h"
#include "device.h"
#include "process.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Finds the debuggable WebView sockets on the device with the given serial.
 *
 * Runs "adb -s <serial> shell cat /proc/net/unix" and keeps only the lines
 * ending in a _devtools_remote socket name. The serial is passed explicitly
 * because /proc/net/unix is read per device, so this works when more than one
 * device is connected.
 *
 * The socket name is the last field on the line, so we split each line on its
 * last space. Each result carries both the socket name and the serial it came
 * from, so a caller iterating over several devices never loses track of which
 * device a socket belongs to.
 *
 * `out` must have room for `max` entries. Returns the number of sockets found
 * (possibly zero), or -1 if adb could not be started or memory ran out.
 */
int find_webviews(const char *serial, struct webview *out, int max) {
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "adb -s %s shell cat /proc/net/unix", serial);

  FILE *adb_shell_cat = proc_open(cmd);
  if(!adb_shell_cat) return -1;

  size_t count = 0, capacity = 16;
  char *shell_buffer = malloc(capacity);
  if(!shell_buffer) { proc_close(adb_shell_cat); return -1; }
  shell_buffer[0] = '\0';

  /* Keep only the devtools lines. Everything else in /proc/net/unix is noise. */
  char str_shell[MAX_LEN];
  while (fgets(str_shell, MAX_LEN, adb_shell_cat)) {
    if(strstr(str_shell, "_devtools_remote"))
      shell_buffer = append(shell_buffer, &count, &capacity, str_shell);
  }

  char *sp = shell_buffer;
  char *line_start = sp;
  int c = 0;
  while (*sp) {
    if (*sp == '\n') {
      /* Terminate the current line so strrchr stays inside it, then restore
       * the newline afterwards so the buffer can be freed as one block. */
      *sp = '\0';
      char *last = strrchr(line_start, ' ');
      if (last != NULL && c < max) {
        char *socket_name = last + 1;   /* skip the space */
        strncpy(out[c].socket, socket_name, WV_NAME_LEN - 1);
        out[c].socket[WV_NAME_LEN - 1] = '\0';
        strncpy(out[c].device_id, serial, D_ILEN - 1);
        out[c].device_id[D_ILEN - 1] = '\0';
        c++;
      }
      *sp = '\n';
      line_start = sp + 1;
    }
    sp++;
  }

  free(shell_buffer);
  proc_close(adb_shell_cat);
  return c;
}
