#ifndef LOG_AND_ERR_UTIL_H
#define LOG_AND_ERR_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define DEBUG_MODE 1

void checkFailed(int line, const char *file);

#define debug(...) { if (DEBUG_MODE) { fprintf(stdout, "DEBUG: " __VA_ARGS__); fprintf(stdout, "\n"); } }
#define ok(...) { fprintf(stdout, "OK: " __VA_ARGS__); fprintf(stdout, "\n"); }
#define fatal(...) { fprintf(stderr, "FATAL: " __VA_ARGS__); fprintf(stderr, "\n"); exit(-1); }
#define err(...) { \
  fprintf(stderr, "ERROR: " __VA_ARGS__); \
  fprintf(stderr, "\n"); \
}

#define CHECK(x) if (!(x)) { checkFailed(__LINE__, __FILE__); }
#define CHECK_NONNEG(x) if ((x) < 0) { checkFailed(__LINE__, __FILE__); }
#define CHECK_ZERO(x) if ((x) != 0) { checkFailed(__LINE__, __FILE__); }

#define WouldBlock() (errno == EWOULDBLOCK || errno == EAGAIN)
#define WorthRetrying() (errno == EAGAIN || errno == EINTR)

#endif
