#include <linux/videodev2.h>
#include <libv4l2.h>
#include <stdio.h>
#include <fcntl.h>
#include <string>

#include "log_and_err_util.h"
#include "video_wrappers.h"

using std::string;

int main(int argc, char **argv) {
  debug("Opening device.");
  CaptureSource source("/dev/video1");
  source.InitOrDie();
  debug("Initialised.");
  string filename("/tmp/crap.raw");
  int f = open(filename.c_str(), O_WRONLY);
  if (f < 0) {
    perror("You is dead.");
    return -1;
  }
  const int bufsize = 8 * 640 * 480;
  for (int i = 0; i < 100; ++i) {
    void *buf = malloc(bufsize);
    size_t bytes = source.ReadFrame(buf, bufsize);
    printf("Wrote %ld bytes.", write(f, buf, bytes));
  }
  close(f);
}
