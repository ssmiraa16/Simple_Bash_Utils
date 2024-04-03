#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    no_arguments();
  } else {
    arguments(argc, argv);
  }
  return 0;
}

void no_arguments() { printf("Usage: grep [OPTION]... PATTERNS [FILE]...\n"); }

void arguments(int argc, char *argv[]) {
  options flags = read_options(argc, argv);
  regex_t re;
  int error = regcomp(&re, flags.pattern, REG_EXTENDED | flags.i);
  if (error != 0) {
    printf("error");
  }
  for (int i = optind; i < argc; i++) {
    file_with_flags(argv[i], &re, flags);
  }
  free(flags.pattern);
  regfree(&re);
}

void file_with_flags(char *file_name, regex_t *re, options flags) {
  FILE *f = fopen(file_name, "r");
  if (f != NULL) {
    char *line = NULL;
    size_t len = 0;
    int read = 0;
    int counter = 1;
    int count_match = 0;
    read = getline(&line, &len, f);
    while (read != EOF) {
      int result = regexec(re, line, 0, NULL, 0);
      if ((result == 0 && flags.v == 0) || (flags.v > 0 && result != 0)) {
        if (flags.c == 0 && flags.l == 0) {
          if (flags.h == 0) {
            printf("%s:", file_name);
          }
          if (flags.n > 0) {
            printf("%d:", counter);
          }
          if (flags.o > 0) {
            match_line(re, line);
          } else {
            print_line(line, read);
          }
        }
        count_match++;
      }
      read = getline(&line, &len, f);
      counter++;
    }
    free(line);
    if (flags.c > 0 && flags.l == 0) {
      if (flags.h == 0) {
        printf("%s:", file_name);
      }
      printf("%d\n", count_match);
    }
    if (flags.l > 0 && count_match > 0) {
      printf("%s\n", file_name);
    }
    fclose(f);
  } else {
    if (flags.s == 0) {
      printf("File not exist\n");
    }
  }
}

void print_line(char *line, int len) {
  for (int i = 0; i < len; i++) {
    printf("%c", line[i]);
  }
  if (line[len - 1] != '\n') {
    printf("%c", '\n');
  }
}

void add_pattern(options *flags, char *pattern) {
  int len = strlen(pattern);
  if (flags->len_pattern == 0) {
    flags->pattern = malloc(1024 * sizeof(char));
    flags->pattern[0] = '\0';
    flags->memory_pattern = 1024;
  }
  if (flags->memory_pattern < flags->len_pattern + len) {
    flags->pattern = realloc(flags->pattern, flags->memory_pattern * 2);
  }
  if (flags->len_pattern != 0) {
    strcat(flags->pattern + flags->len_pattern, "|");
    flags->len_pattern++;
  }
  flags->len_pattern +=
      sprintf(flags->pattern + flags->len_pattern, "(%s)", pattern);
}

void regfile(char *file_name, options *flags) {
  FILE *f = fopen(file_name, "r");
  if (f == NULL) {
    if (flags->s == 0) {
      printf("File not exist");
    }
  } else {
    char *line = NULL;
    size_t mem_len = 0;
    int read = getline(&line, &mem_len, f);
    while (read != EOF) {
      if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
      }
      add_pattern(flags, line);
      read = getline(&line, &mem_len, f);
    }
    free(line);
    fclose(f);
  }
}

void match_line(regex_t *re, char *line) {
  regmatch_t reg;
  int offset = 0;
  while (1) {
    int result = regexec(re, line + offset, 1, &reg, 0);
    if (result != 0) {
      break;
    }
    for (int i = reg.rm_so; i < reg.rm_eo; i++) {
      printf("%c", line[i]);
    }
    printf("%c", '\n');
    offset += reg.rm_eo;
  }
}

options read_options(int argc, char *argv[]) {
  options flags = {0};
  int flag = getopt(argc, argv, "e:ivclnshf:o");
  for (; flag != -1; flag = getopt(argc, argv, "e:ivclnshf:o")) {
    switch (flag) {
      case 'e':
        flags.e = 1;
        add_pattern(&flags, optarg);
        break;
      case 'i':
        flags.i = REG_ICASE;
        break;
      case 'v':
        flags.v = 1;
        break;
      case 'c':
        flags.c = 1;
        break;
      case 'l':
        flags.l = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 'h':
        flags.h = 1;
        break;
      case 'f':
        flags.f = 1;
        regfile(optarg, &flags);
        break;
      case 'o':
        flags.o = 1;
        break;
    }
  }
  if (flags.len_pattern == 0) {
    add_pattern(&flags, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    flags.h = 1;
  }
  return flags;
}