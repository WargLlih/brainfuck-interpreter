#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS_SIZE (1 << 32) - 1
#define MAX_DATA_SIZE (1 << 32) - 1
#define HELP_MESSAGE                                                           \
  "Usage:\n\t %s [option] file\nOptions:\n\t --help\tDisplay usage "           \
  "information.\n\t --dbuffer <n>\tCreate a data buffer with a size "          \
  "of <n> byte(s).\n"

typedef uint8_t bool;

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
uint32_t fsize(FILE *file) {
  fseek(file, 0L, SEEK_END);
  return ftell(file);
}

bool execute_brainfuck();

int main(int argc, char *argv[]) {
  uint32_t dbuffer_size = 0;
  FILE *file = NULL;

  switch (argc) {
  case 1:
    goto PANIC;

  case 2:
    if (!strncmp(argv[1], "--help", sizeof("--help"))) {
      fprintf(stdout, HELP_MESSAGE, argv[0]);
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
    fprintf(stderr, HELP_MESSAGE, argv[0]);
    return EXIT_FAILURE;
  }

  printf("dbuffer_size = %u\n", dbuffer_size);
};
