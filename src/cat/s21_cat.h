#ifndef CAT_H
#define CAT_H
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} options;

options read_options(int argc, char *argv[]);
void no_arguments();
void arguments(int argc, char *argv[]);
void print_file(char *file_name, options flags);
void options_print_nonprintsymb(char current, options flags);
void option_squeeze_blank(FILE *fp);
void options_number_lines(char previous, char current, options flags);
void options_end_lines(char current, options flags);
void tab(char current, options flags);
void control_symb(char current, options flags);
void reg_symb(char current);
void meta(char current, options flags);

#endif