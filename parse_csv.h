#ifndef __H_PARSE_CSV__
#define __H_PARSE_CSV__
#include "dataset.h"
#include <stdio.h>

int parse_csv(FILE *in_file, dataset_t **dataset, char separator,
              int skip_header);
#endif // !__H_PARSE_CSV__
