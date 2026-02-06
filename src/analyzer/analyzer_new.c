#include "../../include/analyzer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int8_t analyzer_new(analyzer_t *self, const options_t *options) {
  self->offset = 0;
  self->binary = fopen(options->filepath, "rb");
  if (NULL == self->binary) {
    perror("fopen");
    return -1;
  }
  fseeko(self->binary, 0, SEEK_END);
  self->limit = ftello(self->binary);
  fseeko(self->binary, 0, SEEK_SET);
  return 0;
}
