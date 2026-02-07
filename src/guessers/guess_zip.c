#include "../../include/analyzer.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ZIP_MAGIC_NUMBER "\x50\x4b\x03\x04"
#define ZIP_HEADER_SIZE 30 /* magic number included */
#define ZIP_EOCD_MAGIC_NUMBER "\x50\x4b\x05\x06"
#define ZIP_EOCD_HEADER_SIZE 22 /* magic number included */

typedef struct s_zip_compression_method {
  uint16_t key;
  const char *value;
} zip_compression_method_t;

static const zip_compression_method_t ZIP_COMPRESSION_METHODS[] = {
    {0, "no compression"},
    {1, "shrunk"},
    {2, "reduced with compression factor 1"},
    {3, "reduced with compression factor 2"},
    {4, "reduced with compression factor 3"},
    {5, "reduced with compression factor 4"},
    {6, "imploded"},
    {8, "deflated"},
    {9, "enhanced deflated"},
    {10, "PKWare DCL imploded"},
    {12, "compressed using BZIP2"},
    {14, "LZMA"},
    {18, "compressed using IBM TERSE"},
    {19, "IBM LZ77 z"},
    {98, "PPMd version I, Rev 1 "},
};

#define TOTAL_ZIP_COMPRESSION_METHODS                                          \
  sizeof(ZIP_COMPRESSION_METHODS) / sizeof(zip_compression_method_t)

static const char *find_zip_compression_method(uint16_t key) {
  uint8_t i = 0;

  for (; i < TOTAL_ZIP_COMPRESSION_METHODS; ++i) {
    if (ZIP_COMPRESSION_METHODS[i].key == key) {
      return ZIP_COMPRESSION_METHODS[i].value;
    }
  }
  return "Unknowned compression method";
}

static uint8_t print_zip_info(analyzer_t *analyzer, off_t cursor,
                              const char *compression_method) {
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
  printf("0x%016llx ZIP archive (%s)\n", analyzer->offset, compression_method);
  printf("0x%016llx End of ZIP archive (%llu bytes)\n",
         (uint64_t)analyzer->offset + blob_size, blob_size);
  analyzer->offset += blob_size;
  return 1;
}

uint8_t guess_zip(analyzer_t *analyzer) {
  uint8_t magic[4] = {0};
  off_t cursor = 0;
  uint8_t zip_header[ZIP_HEADER_SIZE] = {0};
  const char *compression_method = "Unknown compression method";

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
      0 != memcmp(magic, ZIP_EOCD_MAGIC_NUMBER, 4)) {
    return 0;
  }
  if (0 == memcmp(magic, ZIP_MAGIC_NUMBER, 4)) {
    if ((ZIP_HEADER_SIZE - 4) != fread(zip_header + 4, sizeof(uint8_t),
                                       ZIP_HEADER_SIZE - 4, analyzer->binary)) {
      perror("fread");
      return 0;
    }
    compression_method = find_zip_compression_method(
        (uint16_t)(*(zip_header + 8) | (*(zip_header + 9) << 4)));
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
    if (0 != memcmp(magic, ZIP_EOCD_MAGIC_NUMBER, 4)) {
      ++cursor;
      continue;
    }
    return print_zip_info(analyzer, cursor, compression_method);
  }
  return 0;
}
