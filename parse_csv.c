#include "parse_csv.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 65536

int csv_calc_size(FILE *in_file, char separator, int skip_header, size_t *m,
                  size_t *n) {
  *m = 0;
  *n = 0;

  char buf[BUF_SIZE];
  int is_first_row = 1;
  size_t cur_row_n = 0;
  ssize_t read = 0;
  while ((read = fread(buf, 1, BUF_SIZE, in_file)) > 0) {
    for (ssize_t i = 0; i < read; i++) {
      if (buf[i] == separator) {
        if (is_first_row) {
          (*n)++;
        }
        cur_row_n++;
      }

      if (buf[i] == '\n' && cur_row_n > 0) {
        if (!is_first_row && cur_row_n != *n) {
          fprintf(
              stderr,
              "Number of columns in row %zu is different from the first row\n",
              *m);
          return EXIT_FAILURE;
        }

        (*m)++;
        is_first_row = 0;
        cur_row_n = 0;
      }
    }
  }

  if (*n > 0)
    (*n)++;
  if (skip_header && *m > 0)
    (*m)--;

  if (read < 0) {
    perror("file read failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int parse_csv(FILE *in_file, dataset_t **dataset, char separator,
              int skip_header) {
  char delim[2] = {separator, 0};

  size_t m = 0, n = 0;
  if (csv_calc_size(in_file, separator, skip_header, &m, &n) != EXIT_SUCCESS) {
    fprintf(stderr, "Failed to parse csv file.\n");
    return EXIT_FAILURE;
  }

  fseek(in_file, 0, SEEK_SET);

  *dataset = dataset_init(m, n);
  if (*dataset == NULL) {
    fprintf(stderr, "Dataset too big to fit in memory.\n");
    return EXIT_FAILURE;
  }

  if (m == 0 || n == 0)
    return EXIT_SUCCESS;

  char b[BUF_SIZE];
  char *line;
  size_t i = 0;
  size_t is_header_line = 1;
  while ((line = fgets(b, BUF_SIZE, in_file)) != NULL) {
    if (skip_header && is_header_line) {
      is_header_line = 0;
      continue;
    }

    char *t = NULL;
    char *saveptr = line;
    while ((t = strtok_r(saveptr, delim, &saveptr)) != NULL) {
      char *endptr;
      double d = strtod(t, &endptr);
      if (*endptr != 0 && !isspace(*endptr)) {
        fprintf(stderr, "Failed to parse '%s' in line %zu as double\n", t, n);
        return EXIT_FAILURE;
      }

      (*dataset)->examples[i++] = d;
    }
  }
  if (ferror(in_file)) {
    perror("file read failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
