#ifndef __H_DATASET__
#define __H_DATASET__
#include <stddef.h>

typedef struct {
  double **examples;
  size_t m;
  size_t n;
  size_t capacity;
} dataset_t;

dataset_t *dataset_init(size_t initial_m, size_t n);
void dataset_calculate_means_stddevs(dataset_t *dataset, double **means,
                                     double **stddevs);
void dataset_normalize(dataset_t *dataset, double *means, double *stddevs);
void dataset_extend(dataset_t *dataset);
void dataset_print(dataset_t *dataset);
void dataset_fini(dataset_t *dataset);

#endif // !__H_DATASET__
