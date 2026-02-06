#include "../../include/analyzer.h"
#include <_stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ZIP_MAGIC_NUMBER "\x50\x4b\x03\x04"
#define ZIP_EOCD "\x50\x4b\x05\x06"
#define ZIP_EOCD_HEADER_SIZE 22

static uint8_t print_zip_info(analyzer_t *analyzer, off_t cursor) {
  uint64_t blob_size = 0;
  uint16_t comment_len = 0;
  uint8_t eocd[ZIP_EOCD_HEADER_SIZE] = {0};

  if (18 != fread(eocd + 4, sizeof(uint8_t), 18, analyzer->binary)) {
    perror("fread");
    return 0;
  }
  comment_len = (uint16_t)(eocd[20] | (eocd[21] << 8));
  if (cursor + ZIP_EOCD_HEADER_SIZE + comment_len > analyzer->limit) {
    return 0;
  }
  blob_size = (uint64_t)(cursor + ZIP_EOCD_HEADER_SIZE + comment_len) -
              (uint64_t)analyzer->offset;
  printf("0x%016llx ZIP archive\n", analyzer->offset);
  printf("0x%016llx End of ZIP archive (%llu bytes)\n",
         (uint64_t)analyzer->offset + blob_size, blob_size);
  analyzer->offset += blob_size;
  return 1;
}

uint8_t guess_zip(analyzer_t *analyzer) {
  uint8_t magic[4] = {0};
  off_t cursor = 0;

  if ((analyzer->offset + 4) > analyzer->limit) {
    return 0;
  }
  if (-1 == fseeko(analyzer->binary, analyzer->offset, SEEK_SET)) {
    perror("fseeko");
    return 0;
  }
  if (4 != fread(magic, sizeof(uint8_t), 4, analyzer->binary)) {
    perror("fread");
    return 0;
  }
  if (0 != memcmp(magic, ZIP_MAGIC_NUMBER, 4) &&
      0 != memcmp(magic, ZIP_EOCD, 4)) {
    return 0;
  }
  cursor = (analyzer->offset + 4);
  while (cursor + 22 <= analyzer->limit) {
    if (-1 == fseeko(analyzer->binary, cursor, SEEK_SET)) {
      perror("fseeko");
      return 0;
    }
    if (4 != fread(magic, sizeof(uint8_t), 4, analyzer->binary)) {
      perror("fread");
      return 0;
    }
    if (0 != memcmp(magic, ZIP_EOCD, 4)) {
      ++cursor;
      continue;
    }
    return print_zip_info(analyzer, cursor);
  }
  return 0;
}
