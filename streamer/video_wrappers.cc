#include <linux/videodev2.h>
#include <libv4l2.h>
#include <stdio.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <sys/mman.h>

#include "log_and_err_util.h"
#include "video_wrappers.h"

using std::string;
using std::vector;
using std::make_pair;

CaptureSource::CaptureSource(const string &video_device) {
  fd_ = v4l2_open(video_device.c_str(), O_RDWR|O_NONBLOCK);
  CHECK_NONNEG(fd_);
}

CaptureSource::~CaptureSource() {
}

void CaptureSource::InitOrDie() {
  SetCaptureFormat();
  MapBuffers(AllocateBuffers(2));
}

// Just sends an ioctl to the v4l device with a bit of code
// to handle EINTR and EAGAIN retries.
bool CaptureSource::VideoCommand(int command, void *data) {
  while (true) {
    int ret = v4l2_ioctl(fd_, command, data);
    if (ret != -1) {
      return true;
    } else if (WorthRetrying()) {
      continue;
    }
    return false;
  }
  return false;
}

// Sets up the capture format.
void CaptureSource::SetCaptureFormat() {
  v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 640;
  fmt.fmt.pix.height = 480;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
  debug("Setting capture format.");
  if (!VideoCommand(VIDIOC_S_FMT, &fmt)) {
    fatal("Could not set up capture format.");
  }

  // Let's check if it's set correctly.
  if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24) {
    fatal("libv4l2 did not accept requested format. "
	  "Requested %d, got %d.",
	  fmt.fmt.pix.pixelformat, V4L2_PIX_FMT_RGB24);
  }

  if (fmt.fmt.pix.width != 640 || fmt.fmt.pix.height != 480) {
    err("Driver is not using requested length. "
	"Requested %dx%d, getting %dx%d.",
	640, 480, fmt.fmt.pix.width, fmt.fmt.pix.height);
  }
  ok("Requested capture at %dx%d",
     fmt.fmt.pix.width, fmt.fmt.pix.height);
}

int CaptureSource::AllocateBuffers(int num_buffers) {
  // Allocates the number of V4L buffers specified. Needs to
  // be at least 2 for video capture. Depends on the driver
  // in question.
  v4l2_requestbuffers req;
  req.count = num_buffers;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;  // TODO: Easy, but is it the most efficient?
  if (!VideoCommand(VIDIOC_REQBUFS, &req)) {
    fatal("Could not set up buffers.");
  }
  if (req.count == 0) {
    fatal("We need a nonzero buffer count.");
  }
  ok("Requested %d buffers.", req.count);
  return req.count;
}

void CaptureSource::MapBuffers(int allocated_buffers) {
  CHECK(buffers_.size() == 0);
  for (int i = 0; i < allocated_buffers; ++i) {
    v4l2_buffer buf;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    VideoCommand(VIDIOC_QUERYBUF, &buf);
    void *mapped_ptr = v4l2_mmap(NULL, buf.length,
				 PROT_READ|PROT_WRITE,
				 MAP_SHARED, fd_, buf.m.offset);
    CHECK(mapped_ptr != MAP_FAILED);
    buffers_.push_back(make_pair(mapped_ptr, buf.length));
  }
  ok("Mapped %d buffers.", allocated_buffers);
}
