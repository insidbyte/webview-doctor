#include "webview.h"
#include "device.h"
#include "process.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int find_webviews(const char *serial, struct webview *out, int max) {
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "adb -s %s shell cat /proc/net/unix", serial);
  FILE *adb_shell_cat = proc_open(cmd);
  if(!adb_shell_cat) return -1;
  size_t count = 0, capacity = 16;
  char *shell_buffer = malloc(capacity);
  if(!shell_buffer) return -1;
  shell_buffer[0] = '\0';
  char str_shell[MAX_LEN];
  while (fgets(str_shell, MAX_LEN, adb_shell_cat)) {
    if(strstr(str_shell, "_devtools_remote"))
      shell_buffer = append(shell_buffer, &count, &capacity, str_shell);
  }
  char *sp = shell_buffer; // full buffer from output where substring is _devtools_remote
  char *line_start = sp;
  int c = 0;
  while (*sp) {
    //printf("SP char: %c\n", *sp);
    if (*sp == '\n') {
      *sp = '\0';
      char *last = strrchr(line_start, ' ');
      if (last != NULL && c < max) {
	/* copy the name into the struct, skipping the leading space */
        char *socket_name = last + 1;
        strcpy(out[c].socket, socket_name);
	strcpy(out[c].device_id, serial);
	out[c].socket[WV_NAME_LEN - 1] = '\0';
	c++;
      }
      // reset and assign new char after \n as puntator of line start
      line_start = sp + 1;
      *sp = '\n';
    }
    
    sp++;
  }
  free(shell_buffer);
  proc_close(adb_shell_cat);
  return c;
}
