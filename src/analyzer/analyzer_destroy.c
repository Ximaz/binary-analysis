#include "../../include/analyzer.h"
#include <stdio.h>
#include <stdlib.h>

void analyzer_destroy(analyzer_t *self) {
  self->limit = 0;
  self->offset = 0;
  if (NULL != self->binary) {
    fclose(self->binary);
    self->binary = NULL;
  }
}
