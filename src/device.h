#ifndef DEVICE_H
#define DEVICE_H

#include <stddef.h>

#define MAX_LEN 256
#define D_ILEN 32
#define D_SLEN 32
#define D_MAX 16

struct device {
  char id[D_ILEN];
  char state[D_SLEN];
};
char *append(char *buffer, size_t *count, size_t *capacity, const char *line);
void allocate_d(char *str, struct device *d);
int compute_lines(const char *buffer, struct device *d);
#endif
