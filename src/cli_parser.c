#include "../include/cli_parser.h"
#include <stdint.h>
#include <stdio.h>

int8_t cli_parse_options(options_t *self, int argc, const char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <binary_path>\n", argv[0]);
    return -1;
  }
  self->filepath = argv[1];
  return 0;
}
