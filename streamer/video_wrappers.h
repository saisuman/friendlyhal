#include <string>
#include <vector>

class CaptureSource {
 public:
  CaptureSource(const std::string &video_device);
  ~CaptureSource();

  void InitOrDie();
  size_t ReadFrame(void *target, size_t size);

 private:
  bool VideoCommand(int command, void *data);
  void SetCaptureFormat();
  int AllocateBuffers(int num_buffers);
  void MapBuffers(int allocated_buffers);
  
  // File descriptor for open video device.
  int fd_;
  // Each entry in this vector has a pointer to a
  // memory-mapped buffer and its size.
  std::vector<std::pair<void *, size_t> > buffers_;
};

