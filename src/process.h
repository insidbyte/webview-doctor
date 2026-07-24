#ifndef PROCESS_H
#define PROCESS_H
#include <stddef.h>
#include <stdio.h>
FILE *proc_open(const char *command);   // lancia un comando, apri per leggerne l'output
int   proc_close(FILE *stream);         // chiudilo
#endif
