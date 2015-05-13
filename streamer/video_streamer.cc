#include <linux/videodev2.h>
#include <libv4l2.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <glog/logging.h>
#include "video_wrappers.h"

using std::string;

int main(int argc, char **argv) {
  DLOG(INFO) << "Opening device." ;
  CaptureSource source("/dev/video0");
  source.InitOrDie();
  DLOG(INFO) << "Initialised.";
  string filename("/tmp/crap.raw");
  int f = open(filename.c_str(), O_WRONLY|O_CREAT, S_IROTH);
  CHECK_GE(f, 0) << "Could not open video device.";
  const int bufsize = 8 * 640 * 480;
  unsigned char* buf = new unsigned char[bufsize];
  for (int i = 0; i < 100; ++i) {
    size_t bytes = source.ReadFrame(buf, bufsize);
    size_t written = write(f, buf, bytes);
    DLOG(INFO) << "Wrote %ld bytes." << written;
  }
  delete buf;
  close(f);
}
