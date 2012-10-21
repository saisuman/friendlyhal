#include <string>
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


#define ok(...) fprintf(stdout, "OK: " __VA_ARGS__)
#define fatal(...) { fprintf(stderr, "FATAL: " __VA_ARGS__); exit(-1); }
#define err(...) fprintf(stderr, "ERROR: " __VA_ARGS__)

#define CHECK_NONNEG(x) if ((x) < 0) { checkFailed(__LINE__, __FILE__); }
#define CHECK_ZERO(x) if ((x) != 0) { checkFailed(__LINE__, __FILE__); }

void checkFailed(int line, const char *file) {
  fprintf(stderr, "CHECK_FAILED: At %s:%d. Error:%s.",
	  file, line, strerror(errno));
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

class VideoStreamer {
public:
  VideoStreamer(const string &camera_device, int port)
    : camera_device_(camera_device), port_(port) {
    pthread_mutex_init(&socket_lock_, NULL);
  }

  void Start();
  void CloseAndRemove(int fd);
  set<int> GetSockets() { return client_sockets_; }  // UGH.

private:
  void Serve();
  void NewStream(int fd);
  int port_;
  const string &camera_device_;
  int server_socket_;
  pthread_mutex_t socket_lock_;
  set<int> client_sockets_;
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

void VideoSource::Start() {
  // Create a pipe first.
  int stderrpipe[2];
  CHECK_ZERO(pipe2(stderrpipe, O_NONBLOCK));
  int stdoutpipe[2];
  CHECK_ZERO(pipe2(stdoutpipe, O_NONBLOCK));
  ok("Created a pipe to %d, %d and %d, %d.\n",
     stdoutpipe[0], stdoutpipe[1], stderrpipe[0], stderrpipe[1]);
  
  if (fork() != 0) {
    transcoder_stdout_ = stdoutpipe[0];
    transcoder_stderr_ = stderrpipe[0];
    ReadAndBroadcast();
  } else {
    // In the child, we'll replace stdout and stderr.

    ok("Hello, this is the child.\n");
    close(2);
    close(1);
    CHECK_NONNEG(dup(stdoutpipe[1]));
    CHECK_NONNEG(dup(stderrpipe[1]));
    // Now we exec away, and read the output.
    execl("/usr/bin/avconv",
	  "/usr/bin/avconv",
	  "-f", "video4linux2",
	  "-i", "/dev/video3",
	  "-vcodec", "mpeg4",
	  "-f", "mpegts",
	  "-", NULL);
    fatal("exec() actually returned!");
  }
}

void VideoSource::ReadAndBroadcast() {
  char stderrbuf[4096];
  char videobuf[4096];

  int stderr_bufbytes = 0;
  int video_bufbytes = 0;

  while (true) {
    int bread = read(transcoder_stderr_, &stderrbuf, sizeof(stderrbuf));
    if (bread == -1 && WouldBlock()) {
      //ok("Stderr would have blocked, continuing.\n");
    } else if (bread == -1) {
      err("Could not read transcoder's stderr.\n");
    } else if (bread > 0) {
      // err("Transcoder wrote err output: %s\b\n", stderrbuf);
    }

    bread = read(transcoder_stdout_, &videobuf, sizeof(videobuf));
    if (bread == -1 && WouldBlock()) {
      // ok("Stdout would have blocked, continuing.\n");
      continue;
    } else if (bread == -1) {
      fatal("Couldn't read transcoder output.\n");
    } else if (bread == 0) {
      continue;
    }
   
    set<int> fds = streamer_->GetSockets();
    for (set<int>::const_iterator it = fds.begin();
	 it != fds.end(); ++it) {
      int bwritten = write(*it, videobuf, bread);
      if (bwritten == -1 && WouldBlock()) {
	err("Stream %d is falling behind.\n", *it);
      } else if (bwritten == -1) {
	err("Stream %d had an error. Closing.\n", *it);
	streamer_->CloseAndRemove(*it);
      } else {
	ok("Wrote %d bytes out to socket %d.\n", bwritten, *it);
      }
    }
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
  client_sockets_.insert(fd);
}

void VideoStreamer::CloseAndRemove(int fd) {
  MutexLock lock(&socket_lock_);
  client_sockets_.erase(fd);
  ok("Removed socket: %d\n", fd);
}

void* StartStreamer(void *streamer) {
  ok("Kicking off streamer in new thread.\n");
  ((VideoStreamer*)streamer)->Start();
  return NULL;
}

int main(int argc, char **argv) {

  const char *camera_device = argv[1];
  const int port = atoi(argv[2]);

  VideoStreamer streamer(camera_device, port);
  pthread_t streamer_thread;
  CHECK_ZERO(pthread_create(&streamer_thread, NULL, &StartStreamer,
			    (void*)&streamer));
  VideoSource source(&streamer);
  source.Start();
  return 0;
}
