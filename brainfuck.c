#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS_SIZE ((1UL << 32) - 1)
#define MAX_DATA_SIZE ((1UL << 32) - 1)
#define HELP_MESSAGE                                                           \
  "Usage:\n\t %s [option] file\nOptions:\n\t --help\tDisplay usage "           \
  "information.\n\t --dbuffer <n>\tCreate a data buffer with a size "          \
  "of <n> byte(s); 0 < n <= %lu\n"

typedef uint8_t bool;

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
uint64_t fsize(FILE *file) {
  fseek(file, 0L, SEEK_END);
  return ftell(file);
}

bool execute_brainfuck(uint8_t *data_buffer, uint8_t *instructions) {}

int main(int argc, char *argv[]) {
  uint32_t dbuffer_size = 0;
  FILE *file = NULL;

  switch (argc) {
  case 1:
    goto PANIC;

  case 2:
    if (!strncmp(argv[1], "--help", sizeof("--help"))) {
      fprintf(stdout, HELP_MESSAGE, argv[0], MAX_DATA_SIZE);
      return EXIT_SUCCESS;
    }
    goto PANIC;

  case 4:
    // if it is NOT --dbuffer, PANIC
    if (strncmp(argv[1], "--dbuffer", sizeof("--dbuffer"))) {
      goto PANIC;
    }
    dbuffer_size = strtol(argv[2], NULL, 0);
    if (dbuffer_size == 0) {
      fprintf(stderr, "the param <n> must be a int > 0. Got: %s\n", argv[2]);
      return EXIT_FAILURE;
    }
    file = fopen(argv[3], "r");
    if (file == NULL) {
      fprintf(stderr, "Failed to open %s\n", argv[3]);
      return EXIT_FAILURE;
    }
    break;

  PANIC:
  default:
    fprintf(stderr, HELP_MESSAGE, argv[0], MAX_DATA_SIZE);
    return EXIT_FAILURE;
  }

  uint64_t file_size = fsize(file);
  if (file_size > MAX_INSTRUCTIONS_SIZE) {
    fprintf(stderr, "max supported file size is %lu bytes\n",
            MAX_INSTRUCTIONS_SIZE);
    return EXIT_FAILURE;
  }

  uint8_t *instructions = (uint8_t *)malloc(file_size);
  if (instructions == NULL) {
    fprintf(stderr, "Failed to allocate %lu bytes\n", file_size);
    return EXIT_FAILURE;
  }

  uint8_t *data_buffer = (uint8_t *)malloc(dbuffer_size);
  if (data_buffer == NULL) {
    fprintf(stderr, "Failed to allocate %u bytes\n", dbuffer_size);
    return EXIT_FAILURE;
  }

  fgets((char *)instructions, file_size, file);
};
