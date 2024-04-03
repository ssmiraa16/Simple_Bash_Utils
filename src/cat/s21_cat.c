#include "s21_cat.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    no_arguments();
  } else {
    arguments(argc, argv);
  }
  return 0;
}

void no_arguments() {
  char buff[4096];
  int bytes = read(0, buff, 4096);
  while (bytes != -1) {
    printf("%.*s", bytes, buff);
    bytes = read(0, buff, 4000);
  }
}

void arguments(int argc, char *argv[]) {
  options flags = read_options(argc, argv);
  if (flags.b == 1) {
    flags.n = 0;
  }
  for (int i = optind; i < argc; i++) {
    print_file(argv[i], flags);
  }
}

void print_file(char *file_name, options flags) {
  FILE *f = fopen(file_name, "r");
  if (f != NULL) {
    static char previous = '\n';
    char current = fgetc(f);
    while (!feof(f)) {
      options_number_lines(previous, current, flags);
      options_end_lines(current, flags);
      options_print_nonprintsymb(current, flags);
      if (flags.s > 0 && previous == '\n' && current == '\n') {
        option_squeeze_blank(f);
      }
      previous = current;
      current = fgetc(f);
    }
    fclose(f);
  } else {
    printf("File not exist\n");
  }
}

void option_squeeze_blank(FILE *fp) {
  char current = getc(fp);
  while (current == '\n') {
    current = getc(fp);
  }
  ungetc(current, fp);
}

void options_number_lines(char previous, char current, options flags) {
  static int count = 0;
  if (flags.b > 0 && previous == '\n' && current != '\n') {
    fprintf(stdout, "%6d\t", ++count);
  } else if (flags.n > 0 && previous == '\n') {
    fprintf(stdout, "%6d\t", ++count);
  }
}

void options_end_lines(char current, options flags) {
  if (flags.e > 0 && (current == '\n' || current == EOF)) {
    fputc('$', stdout);
  }
}

void options_print_nonprintsymb(char current, options flags) {
  if (current == '\t') {
    tab(current, flags);
  } else if (isprint(current) || current == '\n') {
    reg_symb(current);
  } else if (iscntrl(current)) {
    control_symb(current, flags);
  } else {
    meta(current, flags);
  }
}

void tab(char current, options flags) {
  if (flags.t > 0) {
    reg_symb('^');
    reg_symb(current + 64);
  } else {
    reg_symb(current);
  }
}

void control_symb(char current, options flags) {
  if (flags.v > 0) {
    reg_symb('^');
    if (current == 127) {
      reg_symb(current - 64);
    } else {
      reg_symb(current + 64);
    }
  } else {
    reg_symb(current);
  }
}

void reg_symb(char current) { fputc(current, stdout); }

void meta(char current, options flags) {
  char meta = (signed char)current + 128;
  if ((signed char)current < 0) {
    if (flags.v > 0) {
      reg_symb('M');
      reg_symb('-');
      if (isprint(meta)) {
        reg_symb(meta);
      } else {
        control_symb(meta, flags);
      }
    } else {
      reg_symb(current);
    }
  }
}

options read_options(int argc, char *argv[]) {
  struct option long_option[] = {{"number-nonblank", 0, 0, 'b'},
                                 {"number", 0, 0, 'n'},
                                 {"squeeze-blank", 0, 0, 's'},
                                 {0, 0, 0, 0}};
  int flag = getopt_long(argc, argv, "benstvET", long_option, NULL);
  options flags = {0};
  for (; flag != -1;
       flag = getopt_long(argc, argv, "benstvET", long_option, NULL)) {
    switch (flag) {
      case 'b':
        flags.b = 1;
        break;
      case 'e':
        flags.e = 1;
        flags.v = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 't':
        flags.t = 1;
        flags.v = 1;
        break;
      case 'E':
        flags.e = 1;
        break;
      case 'T':
        flags.t = 1;
        break;
      case 'v':
        flags.v = 1;
        break;
    }
  }
  return flags;
}