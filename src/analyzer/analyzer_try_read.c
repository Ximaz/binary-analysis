#include "../../include/analyzer.h"
#include <stdint.h>

int8_t analyzer_try_read(analyzer_t *self, void *output, uint64_t size,
                         uint64_t count) {
  if ((size * count) > self->limit) {
    return -1;
  }
  if (count != fread(output, size, count, self->binary)) {
    perror("fread");
    return -1;
  }
  self->offset += count * size;
  return 0;
}
