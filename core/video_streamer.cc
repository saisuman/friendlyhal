#include <list>
#include <string>
#include <vector>
#include <set>

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

using std::string;
using std::set;
using std::list;
using std::pair;
using std::vector;

#define ok(...) { fprintf(stdout, "OK: " __VA_ARGS__); }
#define fatal(...) { fprintf(stderr, "FATAL: " __VA_ARGS__); exit(-1); }
#define err(...) fprintf(stderr, "ERROR: " __VA_ARGS__)

#define CHECK(x) if (!(x)) { checkFailed(__LINE__, __FILE__); }
#define CHECK_NONNEG(x) if ((x) < 0) { checkFailed(__LINE__, __FILE__); }
#define CHECK_ZERO(x) if ((x) != 0) { checkFailed(__LINE__, __FILE__); }

void checkFailed(int line, const char *file) {
  fprintf(stderr, "CHECK_FAILED: At %s:%d. Error:%s.",
	  file, line, strerror(errno));
  exit(-1);
}

#define WouldBlock() (errno == EWOULDBLOCK || errno == EAGAIN)

class MutexLock {
public:
  MutexLock(pthread_mutex_t *pmutex) : pmutex_(pmutex) {
    pthread_mutex_lock(pmutex_);
  }
  ~MutexLock() {
    pthread_mutex_unlock(pmutex_);
  }
private:
  pthread_mutex_t *pmutex_;
};

#define PER_SOCKET_BUFFER_LIMIT (1048576 * 4)
class BufferedConnection {
public:
  BufferedConnection(int sockfd)
    : sockfd_(sockfd), start_offset_(0),
      accumulated_bytes_(0) {}
  ~BufferedConnection() {
    ok("Closing connection %d.\n", sockfd_);
    PurgeBuffers();
  }
  bool Send(const char *buffer, int bytes) {
    if (accumulated_bytes_ + bytes > PER_SOCKET_BUFFER_LIMIT) {
      err("Socket %d is falling behind. Dropping %d bytes.\n",
	  sockfd_, accumulated_bytes_);
      PurgeBuffers();
    }

    // Copy this buffer over. TODO: Maybe optimise the copy away
    // for the majority case where you can send right away?
    char *bufcpy = new char[bytes];
    memcpy(bufcpy, buffer, bytes);
    buffers_.push_back(std::make_pair(bufcpy, bytes));
    accumulated_bytes_ += bytes;
    while (buffers_.size() > 0) {
      const char *sendbuf = buffers_.begin()->first + start_offset_;
      const size_t sendbufsize = buffers_.begin()->second - start_offset_;
      int ret = send(sockfd_, sendbuf, sendbufsize,
		     MSG_DONTWAIT | MSG_NOSIGNAL);
      if (ret == -1 && WouldBlock()) {
	// Would have blocked. Get out.
	ok("Would have blocked, accumulating %d and continuing.\n",
	   accumulated_bytes_);
	return true;
      } else if (ret == -1) {
	// Problem. Socket needs to be closed.
	err("Socket %d will need to be closed.\n", sockfd_);
	return false;
      }
      // Some bytes were written. If it's all of it, continue.
      if (ret < sendbufsize) {
	ok("Socket %d wrote only %d bytes, accumulating %d.\n",
	   sockfd_, ret, accumulated_bytes_ - ret);
	start_offset_ += ret;
	return true;
      }
      // No errors, and it means we wrote all of it out. Let's
      // pop that buffer and write out the next one.
      start_offset_ = 0;
      accumulated_bytes_ -= sendbufsize;
      delete buffers_.begin()->first;
      buffers_.pop_front();
    }
    // Wrote off all pending buffers.
    return true;
  }

private:
  void PurgeBuffers() {
    ok("Purging %d bytes of buffers.\n", accumulated_bytes_);
    while (buffers_.size() > 0) {
      delete buffers_.begin()->first;
      buffers_.pop_front();
    }
    accumulated_bytes_ = 0; 
    start_offset_ = 0;
  }
  int sockfd_;
  char buffer_[1048576 * 4];  // 4 MB buffer.
  int start_offset_;
  int accumulated_bytes_;
  list<pair<char *, size_t> > buffers_;
};

class VideoStreamer {
public:
  VideoStreamer(int port) : port_(port) {
    pthread_mutex_init(&socket_lock_, NULL);
  }

  void Start();
  void Broadcast(const char *buf, int bytes);

private:
  void Serve();
  void NewStream(int fd);
  int port_;
  int server_socket_;
  pthread_mutex_t socket_lock_;
  vector<BufferedConnection*> connections_;
};

class VideoSource {
public:
  VideoSource(VideoStreamer *streamer) : streamer_(streamer) {}
  void Start();
private:
  void ReadAndBroadcast();
  int transcoder_stdout_;
  int transcoder_stderr_;
  VideoStreamer *streamer_;
};

void VideoStreamer::Broadcast(const char *buf, int bytes) {
  MutexLock lock(&socket_lock_);
  list<int> offsets_to_remove;
  for (int i = 0; i < connections_.size(); ++i) {
    if (!connections_[i]->Send(buf, bytes)) {
      // Means this needs to be removed.
      offsets_to_remove.push_front(i);
    }
  }
  while (offsets_to_remove.size() > 0) {
    int offset = *offsets_to_remove.begin();
    delete connections_[offset];
    connections_.erase(connections_.begin() + offset);
    offsets_to_remove.pop_front();
  }    
}

void VideoSource::Start() {
  int stdoutpipe[2];
  CHECK_ZERO(pipe2(stdoutpipe, O_NONBLOCK));
  
  if (fork() != 0) {
    transcoder_stdout_ = stdoutpipe[0];
    ReadAndBroadcast();
  } else {
    // In the child, we'll replace stdout and stderr.

    ok("Hello, this is the child.\n");
    close(1);
    CHECK_NONNEG(dup(stdoutpipe[1]));
    // Now we exec away, and read the output.
    execlp("video_source", "video_source", NULL);
    fatal("exec() actually returned!");
  }
}

void VideoSource::ReadAndBroadcast() {
  char videobuf[4096];

  while (true) {
    int bread = read(transcoder_stdout_, &videobuf, sizeof(videobuf));
    if (bread == -1 && WouldBlock()) {
      continue;
    } else if (bread == -1) {
      fatal("Couldn't read transcoder output.\n");
    } else if (bread == 0) {
      continue;
    }
    streamer_->Broadcast(videobuf, bread);
  }
}

void VideoStreamer::Start() {
  ok("Starting.\n");
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

  CHECK_NONNEG(server_socket_);

  int optval = 1;  // SO_REUSEADDR needs int.
  CHECK_ZERO(setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));
  struct sockaddr_in bind_addr;
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port = htons(port_);
  bind_addr.sin_addr.s_addr = INADDR_ANY;
  CHECK_ZERO(bind(server_socket_, (struct sockaddr*)&bind_addr, sizeof(bind_addr)));
  CHECK_ZERO(listen(server_socket_, 10));
  ok("Started, now listening...\n");
  Serve();
}

void VideoStreamer::Serve() {
  while (true) {
    int fd = accept(server_socket_, NULL, NULL);
    if (fd < 0) {
      err("Could not accept socket correctly.\n");
      continue;
    }
    NewStream(fd);
  }
}

void VideoStreamer::NewStream(int fd) {
  MutexLock lock(&socket_lock_);
  ok("Started a new stream: %d\n", fd);
  connections_.push_back(new BufferedConnection(fd));
}

void* StartSource(void *source) {
  ok("Kicking off source in new thread.\n");
  ((VideoSource*)source)->Start();
  return NULL;
}

int main(int argc, char **argv) {

  const int port = atoi(argv[1]);

  VideoStreamer streamer(port);
  VideoSource source(&streamer);
  pthread_t source_thread;
  CHECK_ZERO(pthread_create(&source_thread, NULL, &StartSource,
			    (void*)&source));
  streamer.Start();
  return 0;
}
