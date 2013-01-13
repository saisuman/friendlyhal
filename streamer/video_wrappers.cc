#include <linux/videodev2.h>
#include <libv4l2.h>
#include <stdio.h>
#include <fcntl.h>

#include "log_and_err_util.h"

// Just sends an ioctl to the v4l device with a bit of code
// to handle EINTR and EAGAIN retries.
bool VideoCommand(int video_dev_fd, int command, void *data) {
  while (true) {
    int ret = v4l2_ioctl(video_dev_fd, command, data);
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
void SetCaptureFormat(int video_dev_fd) {
  v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 640;
  fmt.fmt.pix.height = 480;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
  debug("Setting capture format.");
  if (!VideoCommand(video_dev_fd, VIDIOC_S_FMT, &fmt)) {
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
}

int AllocateBuffers(int video_dev_fd, int num_buffers) {
  // Allocates the number of V4L buffers specified. Needs to
  // be at least 2 for video capture. Depends on the driver
  // in question.
  v4l2_requestbuffers req;
  req.count = num_buffers;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;  // TODO: Easy, but is it the most efficient?
  if (!VideoCommand(video_dev_fd, VIDIOC_REQBUFS, &req)) {
    fatal("Could not set up buffers.");
  }
  return req.count;
}

