#pragma once
#include <stdint.h>

// Read a 32-bit positive integer from a given null-terminated string
uint32_t read_u32(const char *s);

uint32_t read_u32(const char *s) {
  uint32_t result = 0;
  while (*s >= '0' && *s <= '9') {
    result = result * 10 + *s - '0';
    ++s;
  }
  return result;
}
