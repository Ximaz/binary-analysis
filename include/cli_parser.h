#ifndef __CLI_PARSER_H_
#define __CLI_PARSER_H_

#include <stdint.h>

typedef struct s_options {
  /* The path to the binary file to analyze. */
  const char *filepath;
} options_t;

/**
 * @brief Parse the CLI arguments into options.
 * @param self The parsed CLI arguments.
 * @param argc The argument count.
 * @param argv The argument vector.
 * @return 0 on success, -1 otherwise.
 */
int8_t cli_parse_options(options_t *self, int argc, const char *argv[]);

#endif /* !__CLI_PARSER_H_ */
