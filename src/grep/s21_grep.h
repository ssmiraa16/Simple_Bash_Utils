#ifndef GREP_H
#define GREP_H
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int s;
  int h;
  int f;
  int o;
  char *pattern;
  int len_pattern;
  int memory_pattern;
} options;

options read_options(int argc, char *argv[]);
void no_arguments();
void arguments(int argc, char *argv[]);
void file_with_flags(char *file_name, regex_t *re, options flags);
void print_line(char *line, int len);
void add_pattern(options *flags, char *pattern);
void regfile(char *file_name, options *flags);
void match_line(regex_t *re, char *line);

#endif