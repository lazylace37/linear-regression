#include "dataset.h"
#include "parse_csv.h"
#include <argp.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 2048

static struct argp_option options[] = {
    {"model", 'm', "FILE", 0, "Model file", 0},
    {"file", 0, "FILE", 0, "Input file", 0},
    {"separator", 's', "separator", 0, "CSV separator (default ';')", 0},
    {"skip-header", 500, NULL, OPTION_ARG_OPTIONAL,
     "Skip CSV header (default false)", 0},
    {0}};

struct arguments {
  char *in_file;
  char *model_file;
  char separator;
  int skip_header;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
  case ARGP_KEY_ARG:
  case 'f':
    arguments->in_file = arg;
    break;
  case 'm':
    arguments->model_file = arg;
    break;
  case 's':
    if (arg != NULL)
      arguments->separator = arg[0];
    break;
  case 500:
    arguments->skip_header = 1;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {
    options, parse_opt, "FILE", "Linear Regression example", NULL, NULL, NULL,
};

double compute_hypothesis(size_t n, double *theta, double *example) {
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
  arguments.in_file = NULL;
  arguments.model_file = NULL;
  arguments.separator = ';';
  arguments.skip_header = 0;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  FILE *in_file = stdin;
  if (arguments.in_file != NULL && strlen(arguments.in_file) > 0) {
    in_file = fopen(arguments.in_file, "r");
  }
  if (!in_file) {
    perror("input file open failed");
    return EXIT_FAILURE;
  }

  FILE *model_file = stdin;
  if (arguments.model_file != NULL && strlen(arguments.model_file) > 0) {
    model_file = fopen(arguments.model_file, "r");
  }
  if (!model_file) {
    perror("model file open failed");
    return EXIT_FAILURE;
  }

  dataset_t *dataset = NULL;
  if (parse_csv(in_file, &dataset, arguments.separator,
                arguments.skip_header) != EXIT_SUCCESS) {
    fprintf(stderr, "Failed to parse input CSV\n");
    dataset_fini(dataset);
    return EXIT_FAILURE;
  }
  if (dataset->m == 0) {
    fprintf(stderr, "No data found\n");
    dataset_fini(dataset);
    return EXIT_FAILURE;
  } else if (dataset->n == 0) {
    fprintf(stderr, "No features found\n");
    dataset_fini(dataset);
    return EXIT_FAILURE;
  }

  dataset_t *model = NULL;
  if (parse_csv(model_file, &model, ';', 0) != EXIT_SUCCESS) {
    fprintf(stderr, "Failed to parse model\n");
    dataset_fini(dataset);
    dataset_fini(model);
    return EXIT_FAILURE;
  }
  if (model->m < 3 || model->n == 0) {
    fprintf(stderr, "Invalid model\n");
    dataset_fini(dataset);
    dataset_fini(model);
    return EXIT_FAILURE;
  }

  if (model->n != dataset->n) {
    fprintf(stderr,
            "Number of features in the dataset (%zu) does does match the model "
            "(%zu)\n",
            dataset->n, model->n);
    dataset_fini(dataset);
    dataset_fini(model);
    return EXIT_FAILURE;
  }

  double *means = model->examples[0];
  double *stddevs = model->examples[1];
  double *theta = model->examples[2];

  dataset_normalize(dataset, means, stddevs);

  for (size_t i = 0; i < dataset->m; i++) {
    double h_i = compute_hypothesis(dataset->n, theta, dataset->examples[i]);
    printf("[%.5zu] %.5f\n", i, h_i);
  }

  if (arguments.in_file != NULL && strlen(arguments.in_file) > 0) {
    fclose(in_file);
  }
  if (arguments.model_file != NULL && strlen(arguments.model_file) > 0) {
    fclose(model_file);
  }
  dataset_fini(dataset);
  dataset_fini(model);

  return EXIT_SUCCESS;
}
