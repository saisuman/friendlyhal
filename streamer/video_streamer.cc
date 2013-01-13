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
  CaptureSource source("/dev/video0");
  source.InitOrDie();
  debug("Initialised.");
}
