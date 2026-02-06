#ifndef __ANALYZER_H_
#define __ANALYZER_H_

#include "cli_parser.h"
#include <stdint.h>
#include <stdio.h>

typedef struct s_analyzer {
  FILE *binary;
  off_t offset;
  off_t limit;
} analyzer_t;

/**
 * @brief Prepare the analyzer based on the given options.
 * @param self The analayzer.
 * @param options The parsed CLI arguments.
 * @return 0 on success, -1 otherwise.
 */
int8_t analyzer_new(analyzer_t *self, const options_t *options);

/**
 * @brief Release all the analyzer memory.
 * @param self The analyzer.
 */
void analyzer_destroy(analyzer_t *self);

/**
 * @brief A function that looks for its magic number / header
 * @details If no match is found, the function is NOT responsible for resetting
 * the analyzer to its original state. It's the caller's responsibility.
 * @param analyzer The analyzer.
 * @return 1 if it matches, 0 otherwise.
 */
typedef uint8_t (*format_guesser_t)(analyzer_t *analyzer);

/**
 * @brief Guess whether the current offset points to a zip archive.
 * @param analyzer The analyzer.
 * @return 1 if it matches, 0 otherwise.
 */
uint8_t guess_zip(analyzer_t *analyzer);

static const format_guesser_t GUESSERS[] = {guess_zip};

#define TOTAL_GUESSERS (sizeof(GUESSERS) / sizeof(format_guesser_t))

#endif /* !__ANALYZER_H_ */
