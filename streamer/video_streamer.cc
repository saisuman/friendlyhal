#include <linux/videodev2.h>
#include <libv4l2.h>
#include <stdio.h>
#include <fcntl.h>

#include "log_and_err_util.h"
#include "video_wrappers.h"


int main(int argc, char **argv) {
  debug("Opening device.");
  int fd = v4l2_open("/dev/video0", O_RDWR|O_NONBLOCK);
  CHECK_NONNEG(fd);

  SetCaptureFormat(fd);
  ok("Set up capture format.");
  AllocateBuffers(fd, 2);
  ok("Setup capture buffers.");
}
