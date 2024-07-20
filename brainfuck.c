#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_INSTRUCTIONS_SIZE ((1UL << 32) - 1)
#define MAX_DATA_SIZE ((1UL << 32) - 1)
#define HELP_MESSAGE                                                           \
  "Usage:\n\t %s [option] file\nOptions:\n\t --help\tDisplay usage "           \
  "information.\n\t --dbuffer <n>\tCreate a data buffer with a size "          \
  "of <n> byte(s); 0 < n <= %lu\n"
#define TRUE 1UL
#define FALSE 0UL

typedef uint8_t bool;

/**
 * @brief This function gets how many byte there are in a file.
 * @param file The file in question.
 * @return The file size in bytes
 */
uint64_t fsize(const char *filename) {
  if (filename == NULL) {
    return 0;
  }
  struct stat file_stats;
  if (stat(filename, &file_stats) < 0) {
    return 0;
  }
  return file_stats.st_size;
}

/**
 * @brief ececute_brainfuck function runs the braifuck code.
 *
 * @param data_buffer is a one-dimensional array that holds the program data.
 * @param data_size is how many bytes there are in data_buffer.
 * @param instructions is a one-dimensional array that holds the program
 * instructions.
 * @param inst_size is how many bytes there are in the instructions.
 *
 * @return TRUE if program runs successfully, FALSE otherwise.
 */
bool execute_brainfuck(uint8_t *data_buffer, uint32_t data_size,
                       uint8_t *instructions, uint32_t inst_size) {
  uint32_t iptr, dptr = 0;

  for (iptr = 0; iptr < inst_size; iptr++) {
    switch (instructions[iptr]) {
    case '>':
      if (dptr == data_size - 1) {
        fprintf(
            stderr,
            "[ERROR] Can not increment the data pointer: Overflow prevented.\n"
            "instruction pointer: %u\n",
            iptr);
        return FALSE;
      }
      dptr++;
      break;

    case '<':
      if (dptr == 0) {
        fprintf(
            stderr,
            "[ERROR] Can not decrement the data pointer: Underflow prevented.\n"
            "instruction pointer: %u\n",
            iptr);
        return FALSE;
      }
      dptr--;
      break;

    case '+':
      data_buffer[dptr]++;
      break;

    case '-':
      data_buffer[dptr]--;
      break;

    case '.':
      fprintf(stdout, "%c", data_buffer[dptr]);
      break;

    case ',':
      data_buffer[dptr] = getc(stdin);
      break;

    case '[': {
      if (data_buffer[dptr] == 0) {
        // Ideia: start looking forwards for your match, whenever you found
        // a '[' increment the missmatch var, so you'll know there will be
        // a missmatch in your search meaning you should keep looking.
        uint32_t missmatch = 0;
        while (TRUE) {
          // sanity check
          if (iptr == inst_size) {
            fprintf(stderr, "[ERROR] Could never found a match for '['\n");
            return FALSE;
          }
          iptr++;
          if (instructions[iptr] == '[') {
            missmatch++;
            continue;
          }
          if (instructions[iptr] == ']') {
            if (missmatch == 0) {
              break;
            }
            missmatch--;
          }
        }
      }
      break;
    }

    case ']': {
      if (data_buffer[dptr] != 0) {
        // Ideia: start looking backwards for your match, whenever you found
        // a ']' increment the missmatch var, so you'll know there will be
        // a missmatch in your search meaning you should keep looking.
        uint32_t missmatch = 0;
        while (TRUE) {
          // sanity check
          if (iptr == 0) {
            fprintf(stderr, "[ERROR] Could never found a match for ']'\n");
            return FALSE;
          }
          iptr--;
          if (instructions[iptr] == ']') {
            missmatch++;
            continue;
          }
          if (instructions[iptr] == '[') {
            if (missmatch == 0) {
              break;
            }
            missmatch--;
          }
        }
      }
      break;
    }

    default:
      continue;
    }
  }
  return TRUE;
}

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
    long n = strtol(argv[2], NULL, 0);
    if (n <= 0) {
      fprintf(stderr, "the param <n> must be a int > 0. Got: %s\n", argv[2]);
      return EXIT_FAILURE;
    }
    dbuffer_size = n;
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

  size_t file_size = fsize(argv[3]);
  if (file_size > MAX_INSTRUCTIONS_SIZE) {
    fclose(file);
    fprintf(stderr, "max supported file size is %lu bytes\n",
            MAX_INSTRUCTIONS_SIZE);
    return EXIT_FAILURE;
  }

  uint8_t *instructions = (uint8_t *)malloc(file_size);
  if (instructions == NULL) {
    fclose(file);
    fprintf(stderr, "Failed to allocate %lu bytes\n", file_size);
    return EXIT_FAILURE;
  }

  size_t n = fread(instructions, sizeof(uint8_t), file_size, file);
  if (n != file_size) {
    free(instructions);
    fprintf(stderr, "Error on read file\n");
    fclose(file);
    return EXIT_FAILURE;
  }

  fclose(file);

  uint8_t *data_buffer = (uint8_t *)malloc(dbuffer_size);
  if (data_buffer == NULL) {
    free(instructions);
    fprintf(stderr, "Failed to allocate %u bytes\n", dbuffer_size);
    return EXIT_FAILURE;
  }

  memset(data_buffer, 0, dbuffer_size);

  bool success =
      execute_brainfuck(data_buffer, dbuffer_size, instructions, file_size);

  free(instructions);
  free(data_buffer);

  if (success) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
};
