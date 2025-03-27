#include "dataset.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

dataset_t *dataset_init(size_t m, size_t n) {
  double *examples = malloc(n * m * sizeof(double));
  if (!examples) {
    return NULL;
  }

  dataset_t *dataset = calloc(1, sizeof(dataset_t));
  dataset->examples = examples;
  dataset->m = m;
  dataset->n = n;
  return dataset;
}

void dataset_calculate_means_stddevs(dataset_t *dataset, double **means,
                                     double **stddevs) {
  (*means) = calloc(dataset->n - 1, sizeof(double));
  for (size_t j = 0; j < dataset->n - 1; j++) {
    for (size_t i = 0; i < dataset->m; i++) {
      (*means)[j] += dataset->examples[i * dataset->n + j];
    }
    (*means)[j] /= dataset->m;
  }

  (*stddevs) = calloc(dataset->n - 1, sizeof(double));
  for (size_t j = 0; j < dataset->n - 1; j++) {
    for (size_t i = 0; i < dataset->m; i++) {
      double diff = dataset->examples[i * dataset->n + j] - (*means)[j];
      (*stddevs)[j] += diff * diff;
    }
    (*stddevs)[j] = sqrt((*stddevs)[j] / dataset->m);
  }
}

void dataset_normalize(dataset_t *dataset, double *means, double *stddevs) {
  for (size_t j = 0; j < dataset->n - 1; j++) {
    for (size_t i = 0; i < dataset->m; i++) {
      if (fpclassify(stddevs[j]) != FP_ZERO) {
        dataset->examples[i * dataset->n + j] =
            (dataset->examples[i * dataset->n + j] - means[j]) / stddevs[j];
      }
    }
  }
}

void dataset_split(dataset_t *dataset, double ratio, unsigned int seed,
                   dataset_t **train_dataset, dataset_t **valid_dataset) {
  dataset_t *train = calloc(1, sizeof(dataset_t));
  train->m = dataset->m * ratio;
  train->n = dataset->n;
  train->capacity = 0;

  dataset_t *valid = calloc(1, sizeof(dataset_t));
  valid->m = dataset->m - train->m;
  valid->n = dataset->n;
  valid->capacity = 0;

  for (size_t i = dataset->m - 1; i > 0; i--) {
    size_t j = rand_r(&seed) % (i + 1);

    // Swap the two rows
    for (size_t k = 0; k < dataset->n; k++) {
      double tmp = dataset->examples[i * dataset->n + k];
      dataset->examples[i * dataset->n + k] =
          dataset->examples[j * dataset->n + k];
      dataset->examples[j * dataset->n + k] = tmp;
    }
  }

  train->examples = dataset->examples;
  valid->examples = dataset->examples + train->m * dataset->n;

  (*train_dataset) = train;
  (*valid_dataset) = valid;
}

void dataset_print(dataset_t *dataset) {
  for (size_t i = 0; i < dataset->m; i++) {
    for (size_t j = 0; j < dataset->n; j++) {
      printf("%.5e ", dataset->examples[i * dataset->n + j]);
    }
    printf("\n");
  }
}

void dataset_fini(dataset_t *dataset) {
  if (!dataset)
    return;

  free(dataset->examples);
  free(dataset);
}
