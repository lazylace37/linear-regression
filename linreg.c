#include "dataset.h"
#include "parse_csv.h"
#include <argp.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct argp_option options[] = {
    {"learning-rate", 'l', "learning_rate", 0,
     "The learning rate (default 0.001)", 0},
    {"iterations", 'i', "iterations", 0, "Number of iterations (default 1000)",
     0},
    {"separator", 's', "separator", 0, "CSV separator (default ';')", 0},
    {"skip-header", 500, NULL, OPTION_ARG_OPTIONAL,
     "Skip CSV header (default false)", 0},
    {"file", 600, "FILE", 0, "Input file", 0},
    {"output", 'o', "OUTFILE", 0, "Output model file", 0},
    {0}};

struct arguments {
  double learning_rate;
  size_t iterations;
  char separator;
  int skip_header;
  char *in_file;
  char *out_file;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
  case 'l':
    if (arg != NULL)
      arguments->learning_rate = atof(arg);
    break;
  case 'i':
    if (arg != NULL)
      arguments->iterations = atoi(arg);
    break;

  case 's':
    if (arg != NULL)
      arguments->separator = arg[0];
    break;
  case 500:
    arguments->skip_header = 1;
    break;

  case 600:
  case ARGP_KEY_ARG:
    arguments->in_file = arg;
    break;
  case 'o':
    arguments->out_file = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {
    options, parse_opt, "FILE", "Linear Regression example", NULL, NULL, NULL,
};

double compute_hypothesis(size_t n, double *theta, dataset_example_t example) {
  double h = 0;
  for (size_t i = 0; i < n; i++) {
    if (i == 0) { // Bias
      h += theta[i];
    } else {
      h += theta[i] * example[i - 1];
    }
  }
  return h;
}

int main(int argc, char *argv[]) {
  struct arguments arguments;
  arguments.learning_rate = 0.001;
  arguments.iterations = 1000;
  arguments.separator = ';';
  arguments.skip_header = 0;
  arguments.in_file = NULL;
  arguments.out_file = NULL;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (arguments.separator == 0)
    arguments.separator = ';';

  FILE *in_file = stdin;
  if (arguments.in_file != NULL && strlen(arguments.in_file) > 0) {
    in_file = fopen(arguments.in_file, "r");
  }
  if (!in_file) {
    perror("input file open failed");
    return EXIT_FAILURE;
  }

  dataset_t *dataset = NULL;
  if (parse_csv(in_file, &dataset, arguments.separator,
                arguments.skip_header) != EXIT_SUCCESS) {
    fprintf(stderr, "Failed to parse CSV\n");
    dataset_fini(dataset);
    return EXIT_FAILURE;
  }

  if (dataset->m == 0) {
    fprintf(stderr, "No data found\n");
    dataset_fini(dataset);
    return EXIT_FAILURE;
  } else if (dataset->n < 2) {
    fprintf(stderr, "No features found\n");
    dataset_fini(dataset);
    return EXIT_FAILURE;
  }

  /*printf("Parsed dataset: %zu examples, %zu features\n", dataset->m,*/
  /*       dataset->n);*/
  /*dataset_print(dataset);*/

  // Normalize dataset
  double *means = NULL, *stddevs = NULL;
  dataset_calculate_means_stddevs(dataset, &means, &stddevs);
  dataset_normalize(dataset, means, stddevs);
  dataset_print(dataset);

  /* Linear regression */
  double *theta = calloc(dataset->n, sizeof(double)); // Init weights to 0
  double alpha = arguments.learning_rate;
  size_t iterations = arguments.iterations;

  // Gradient descent
  double *tmp = calloc(dataset->n, sizeof(double));
  for (size_t iter = 0; iter < iterations; iter++) {
    for (size_t j = 0; j < dataset->n; j++) {
      double sum = 0;
      for (size_t i = 0; i < dataset->m; i++) {
        double h_i =
            compute_hypothesis(dataset->n, theta, dataset->examples[i]);
        double y_i = dataset->examples[i][dataset->n - 1];
        if (j == 0)
          sum += h_i - y_i * 1;
        else
          sum += (h_i - y_i) * dataset->examples[i][j - 1];
      }
      double delta = alpha * sum / dataset->m;
      tmp[j] = theta[j] - delta;
    }

    // Update thetas
    for (size_t j = 0; j < dataset->n; j++) {
      theta[j] = tmp[j];
    }

    // Compute cost
    double cost = 0;
    for (size_t i = 0; i < dataset->m; i++) {
      double h_i = compute_hypothesis(dataset->n, theta, dataset->examples[i]);
      double y_i = dataset->examples[i][dataset->n - 1];
      cost += pow(h_i - y_i, 2);
    }
    cost /= (2 * dataset->m);
    printf("[%04zu] %f\n", iter, cost);
  }
  free(tmp);

  // Print thetas
  printf("Thetas:\n[");
  for (size_t j = 0; j < dataset->n; j++) {
    printf("%.5e", theta[j]);
    if (j < dataset->n - 1) {
      printf(", ");
    }
  }
  printf("]\n");

  // Save model
  FILE *out_file = stderr;
  if (arguments.out_file != NULL && strlen(arguments.out_file) > 0) {
    out_file = fopen(arguments.out_file, "w");
  }
  for (size_t i = 0; i < dataset->n - 1; i++) {
    fprintf(out_file, "%.*f;", DBL_DIG - 1, means[i]);
  }
  fprintf(out_file, "0\n");
  for (size_t i = 0; i < dataset->n - 1; i++) {
    fprintf(out_file, "%.*f;", DBL_DIG - 1, stddevs[i]);
  }
  fprintf(out_file, "0\n");
  for (size_t i = 0; i < dataset->n; i++) {
    fprintf(out_file, "%.*f", DBL_DIG - 1, theta[i]);
    if (i < dataset->n - 1)
      fprintf(out_file, ";");
  }
  fprintf(out_file, "\n");

  // Cleanup
  if (strlen(arguments.in_file) > 0)
    fclose(in_file);
  free(theta);
  free(means);
  free(stddevs);
  dataset_fini(dataset);

  return EXIT_SUCCESS;
}
