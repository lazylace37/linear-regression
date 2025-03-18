#include "parse_csv.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 2048

int get_num_features(FILE *in_file, size_t *n, const char *delim) {
  (*n) = 0;

  char b[BUF_SIZE];
  char *line = fgets(b, BUF_SIZE, in_file);
  if (line == NULL)
    return EXIT_FAILURE;

  char *t = NULL;
  char *saveptr = line;
  while ((t = strtok_r(saveptr, delim, &saveptr)) != NULL) {
    (*n)++;
  }
  return EXIT_SUCCESS;
}

int parse_csv(FILE *in_file, dataset_t **dataset, char separator,
              int skip_header) {
  char delim[2] = {separator, 0};

  size_t n = 0;
  if (get_num_features(in_file, &n, delim) != EXIT_SUCCESS) {
    fprintf(stderr, "Failed to parse csv file.\n");
    return EXIT_FAILURE;
  }

  if (!skip_header) {
    fseek(in_file, 0, SEEK_SET);
  }

  *dataset = dataset_init(10, n);
  (*dataset)->n = n;

  char b[BUF_SIZE];
  char *line;
  while ((line = fgets(b, BUF_SIZE, in_file)) != NULL) {
    size_t cur_n = 0;

    char *t = NULL;
    char *saveptr = line;
    while ((t = strtok_r(saveptr, delim, &saveptr)) != NULL) {
      char *endptr;
      double d = strtod(t, &endptr);
      if (*endptr != 0 && !isspace(*endptr)) {
        fprintf(stderr, "Failed to parse '%s' in line %zu as double\n", t, n);
        return EXIT_FAILURE;
      }

      if (cur_n >= (*dataset)->n) {
        fprintf(stderr,
                "Number of features in line %zu is greater than in "
                "line 0\n",
                n);
        return EXIT_FAILURE;
      }

      (*dataset)->examples[(*dataset)->m][cur_n] = d;

      cur_n++;
    }

    (*dataset)->m++;
    if ((*dataset)->m == (*dataset)->capacity) {
      dataset_extend(*dataset);
    }
  }
  if (ferror(in_file)) {
    perror("file read failed");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
