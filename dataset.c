#include "dataset.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

dataset_t *dataset_init(size_t initial_m, size_t n) {
  dataset_t *dataset = calloc(1, sizeof(dataset_t));
  dataset->examples = calloc(initial_m, sizeof(double *));
  dataset->capacity = initial_m;

  for (size_t i = 0; i < initial_m; i++) {
    dataset->examples[i] = calloc(n, sizeof(double));
  }

  return dataset;
}

void dataset_calculate_means_stddevs(dataset_t *dataset, double **means,
                                     double **stddevs) {
  (*means) = calloc(dataset->n - 1, sizeof(double));
  for (size_t j = 0; j < dataset->n - 1; j++) {
    for (size_t i = 0; i < dataset->m; i++) {
      (*means)[j] += dataset->examples[i][j];
    }
    (*means)[j] /= dataset->m;
  }

  (*stddevs) = calloc(dataset->n - 1, sizeof(double));
  for (size_t j = 0; j < dataset->n - 1; j++) {
    for (size_t i = 0; i < dataset->m; i++) {
      double diff = dataset->examples[i][j] - (*means)[j];
      (*stddevs)[j] += diff * diff;
    }
    (*stddevs)[j] = sqrt((*stddevs)[j] / dataset->m);
  }
}

void dataset_normalize(dataset_t *dataset, double *means, double *stddevs) {
  for (size_t j = 0; j < dataset->n - 1; j++) {
    for (size_t i = 0; i < dataset->m; i++) {
      if (fpclassify(stddevs[j]) != FP_ZERO) {
        dataset->examples[i][j] =
            (dataset->examples[i][j] - means[j]) / stddevs[j];
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

    double *tmp = dataset->examples[i];
    dataset->examples[i] = dataset->examples[j];
    dataset->examples[j] = tmp;
  }

  train->examples = dataset->examples;
  valid->examples = dataset->examples + train->m;

  (*train_dataset) = train;
  (*valid_dataset) = valid;
}

void dataset_extend(dataset_t *dataset) {
  dataset->capacity *= 2;
  dataset->examples =
      realloc(dataset->examples, dataset->capacity * sizeof(double *));
  for (size_t i = dataset->m; i < dataset->capacity; i++) {
    dataset->examples[i] = calloc(dataset->n, sizeof(double));
  }
}

void dataset_print(dataset_t *dataset) {
  for (size_t i = 0; i < dataset->m; i++) {
    for (size_t j = 0; j < dataset->n; j++) {
      printf("%.5e ", dataset->examples[i][j]);
    }
    printf("\n");
  }
}

void dataset_fini(dataset_t *dataset) {
  if (!dataset)
    return;

  for (size_t i = 0; i < dataset->capacity; i++) {
    free(dataset->examples[i]);
  }
  free(dataset->examples);
  free(dataset);
}
