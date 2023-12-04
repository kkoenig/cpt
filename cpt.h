#pragma once
#define CPT_DO_CLEANUP 1
#define CPT_HAVE_STAT 1

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if CPT_HAVE_STAT
#include <sys/stat.h>
#endif

// Read a file into a single array
char *cpt_slurp_stdin(size_t *out_opt_size);
char *cpt_slurp_path(const char *path, size_t *out_opt_size);
char *cpt_slurp_fp(FILE *f, size_t *out_opt_size);

// Read a 32-bit positive integer from a given null-terminated string
uint32_t cpt_read_u32(const char *s);

uint32_t cpt_read_u32(const char *s) {
  uint32_t result = 0;
  while (*s >= '0' && *s <= '9') {
    result = result * 10 + *s - '0';
    ++s;
  }
  return result;
}

char *cpt_slurp_stdin(size_t *out_opt_size) {
  return cpt_slurp_fp(stdin, out_opt_size);
}

char *cpt_slurp_path(const char *path, size_t *out_opt_size) {
  FILE *f = fopen(path, "rb");
  setvbuf(f, NULL, _IONBF, 0);
  char *buffer = cpt_slurp_fp(f, out_opt_size);
#if CPT_DO_CLEANUP
  fclose(f);
#endif
  return buffer;
}

char *cpt_slurp_fp(FILE *f, size_t *out_opt_size) {
#if CPT_HAVE_STAT
  struct stat stats;
  (void)fstat(fileno(f), &stats);
  size_t rc = stats.st_size;
#else
  (void)fseek(f, 0, SEEK_END);
  size_t rc = ftell(f);
  (void)fseek(f, 0, SEEK_SET);
#endif

  char *buffer = (char *)malloc(rc);
  size_t bytes_read = fread(buffer, 1, rc, f);
  if (out_opt_size) {
    *out_opt_size = bytes_read;
  }
  return buffer;
}
