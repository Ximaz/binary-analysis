#include "../include/analyzer.h"
#include "../include/cli_parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
  analyzer_t analyzer = {0};
  options_t options = {0};
  uint64_t i = 0;
  uint64_t initial_state = 0;

  if (-1 == cli_parse_options(&options, argc, argv)) {
    return EXIT_FAILURE;
  }
  if (-1 == analyzer_new(&analyzer, &options)) {
    return EXIT_FAILURE;
  }
  while (analyzer.offset < analyzer.limit) {
    initial_state = analyzer.offset;
    for (; i < TOTAL_GUESSERS && 0 == GUESSERS[i](&analyzer); ++i) {
      if (-1 == analyzer_seek_to(&analyzer, initial_state)) {
        analyzer_destroy(&analyzer);
        return EXIT_FAILURE;
      }
    }
    analyzer.offset += (i == TOTAL_GUESSERS);
  }
  analyzer_destroy(&analyzer);
  return EXIT_SUCCESS;
}
