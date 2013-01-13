#include <string>
#include <vector>

class CaptureSource {
 public:
  CaptureSource(const std::string &video_device);
  ~CaptureSource();

  void InitOrDie();

 private:
  bool VideoCommand(int command, void *data);
  void SetCaptureFormat();
  int AllocateBuffers(int num_buffers);
  void MapBuffers(int allocated_buffers);

  int fd_;
  std::vector<std::pair<void *, size_t> > buffers_;
};

