#pragma once
#define CPT_DO_CLEANUP 1
#define CPT_HAVE_STAT 1

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if CPT_HAVE_STAT
#include <sys/stat.h>
#endif

typedef struct {
  const char *data;
  size_t size;
} cpt_buffer;
// todo free

typedef struct {
  const cpt_buffer buffer;
  size_t pos;
} cpt_cursor;
cpt_cursor cpt_cursor_ref(const cpt_buffer);
cpt_cursor cpt_cursor_ref_cstring(const char *);
cpt_cursor cpt_cursor_ref_array(const char *, const size_t size);
bool cpt_cursor_eof(const cpt_cursor);

// Read a file into a single array
cpt_buffer cpt_slurp_stdin();
cpt_buffer cpt_slurp_path(const char *path);
cpt_buffer cpt_slurp_fp(FILE *fp);

typedef struct {
  const cpt_buffer store;
  cpt_buffer *rows;
  size_t num_rows;
  size_t max_column_width;
  size_t min_column_width;
} cpt_buffer2d;
cpt_buffer2d cpt_slurp2d_buffer(cpt_buffer buffer);

// Read a 32-bit positive integer from a given stream
uint32_t cpt_read_u32(cpt_cursor *c);
// Read the next 32-bit positive integer from a given stream, skipping spaces
uint32_t cpt_next_u32(cpt_cursor *c);
// Read a 32-bit integer from a given stream
int32_t cpt_read_i32(cpt_cursor *c);
// Read the next 32-bit integer from a given stream, skipping spaces
int32_t cpt_next_i32(cpt_cursor *c);

// ----

uint32_t cpt_read_u32(cpt_cursor *c) {
  uint32_t result = 0;
  const char *s = c->pos + c->buffer.data;
  while (*s >= '0' && *s <= '9') {
    result = result * 10 + *s - '0';
    ++s;
  }
  c->pos = s - c->buffer.data;
  return result;
}

uint32_t cpt_next_u32(cpt_cursor *c) {
  while (c->buffer.data[c->pos] == ' ' || c->buffer.data[c->pos] == '\n') {
    c->pos++;
  }
  return cpt_read_u32(c);
}

int32_t cpt_read_i32(cpt_cursor *c) {
  const char *s = c->pos + c->buffer.data;
  if (*s != '-') {
    return cpt_read_u32(c);
  }
  ++s;
  int32_t result = 0;
  while (*s >= '0' && *s <= '9') {
    result = result * 10 + *s - '0';
    ++s;
  }
  c->pos = s - c->buffer.data;
  return -result;
}

int32_t cpt_next_i32(cpt_cursor *c) {
  while (c->buffer.data[c->pos] == ' ' || c->buffer.data[c->pos] == '\n') {
    c->pos++;
  }
  return cpt_read_i32(c);
}

bool cpt_cursor_eof(const cpt_cursor c) { return c.pos == c.buffer.size; }

cpt_cursor cpt_cursor_ref_cstring(const char *s) {
  return (cpt_cursor){.buffer = (cpt_buffer){.data = s, .size = strlen(s)},
                      .pos = 0};
}

cpt_cursor cpt_cursor_ref_array(const char *s, const size_t size) {
  return (cpt_cursor){.buffer = (cpt_buffer){.data = s, .size = size},
                      .pos = 0};
}

cpt_cursor cpt_cursor_ref(const cpt_buffer buffer) {
  return (cpt_cursor){.buffer = buffer, .pos = 0};
}

cpt_buffer cpt_slurp_stdin() { return cpt_slurp_fp(stdin); }

cpt_buffer cpt_slurp_path(const char *path) {
  FILE *fp = fopen(path, "rb");
  setvbuf(fp, NULL, _IONBF, 0);
  const cpt_buffer buffer = cpt_slurp_fp(fp);
#if CPT_DO_CLEANUP
  fclose(fp);
#endif
  return buffer;
}

cpt_buffer cpt_slurp_fp(FILE *fp) {
#if CPT_HAVE_STAT
  struct stat stats;
  (void)fstat(fileno(fp), &stats);
  size_t rc = stats.st_size;
#else
  (void)fseek(f, 0, SEEK_END);
  size_t rc = ftell(f);
  (void)fseek(f, 0, SEEK_SET);
#endif

  char *buffer = (char *)malloc(rc + 1);
  size_t bytes_read = fread(buffer, 1, rc, fp);
  buffer[rc] = 0;
  return (cpt_buffer){
      .data = buffer,
      .size = bytes_read,
  };
}

cpt_buffer2d cpt_slurp2d_buffer(cpt_buffer buffer) {
  size_t min_column_size = (size_t)-1;
  size_t max_column_size = 0;
  size_t column_size = 0;
  size_t current_row = 0;
  for (size_t i = 0; i < buffer.size; ++i) {
    if (buffer.data[i] == '\n') {
      ++current_row;
    }
  }

  cpt_buffer *rows = (cpt_buffer *)malloc(sizeof(cpt_buffer) * current_row);
  current_row = 0;
  for (size_t i = 0; i < buffer.size; ++i) {
    if (buffer.data[i] == '\n') {
      if (column_size > max_column_size) {
        max_column_size = column_size;
      }
      if (column_size < min_column_size) {
        min_column_size = column_size;
      }
      rows[current_row++] = (cpt_buffer){
          .data = buffer.data + i - column_size,
          .size = column_size,
      };
      column_size = 0;
      ((char *)buffer.data)[i] = '\0';
    } else {
      ++column_size;
    }
  }
  return (cpt_buffer2d){
      .store = buffer,
      .rows = rows,
      .num_rows = current_row,
      .max_column_width = max_column_size,
      .min_column_width = min_column_size,
  };
}
