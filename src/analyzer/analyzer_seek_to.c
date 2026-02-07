#include "../../include/analyzer.h"
#include <limits.h>
#include <stdio.h>

int8_t analyzer_seek_to(analyzer_t *self, uint64_t offset) {
  if (offset > self->limit) {
    return -1;
  }
  if (-1 == fseek(self->binary, 0, SEEK_SET)) {
    perror("fseek");
    return -1;
  }
  while (offset > LONG_MAX) {
    if (-1 == fseek(self->binary, LONG_MAX, SEEK_CUR)) {
      perror("fseek");
      return -1;
    };
    offset -= LONG_MAX;
  }
  if (offset > 0) {
    if (-1 == fseek(self->binary, offset & 0x7fffffffffffffff, SEEK_CUR)) {
      perror("fseek");
      return -1;
    };
  }
  self->offset = offset;
  return 0;
}
