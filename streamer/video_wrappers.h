bool VideoCommand(int video_dev_fd, int command, void *data);
void SetCaptureFormat(int video_dev_fd);
int AllocateBuffers(int video_dev_fd, int num_buffers);

