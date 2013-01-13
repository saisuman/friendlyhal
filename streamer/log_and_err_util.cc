#include "log_and_err_util.h"

void checkFailed(int line, const char *file) {
  fprintf(stderr, "CHECK_FAILED: At %s:%d. Error:%s.\n",
          file, line, strerror(errno));
  exit(-1);
}
