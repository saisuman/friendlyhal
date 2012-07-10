#include <pocketsphinx.h>
#include <cmd_ln.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define CHECK(c) if (!(c)) {						\
  fprintf(stderr, "***CHECK FAILED*** at %s, %d", __FILE__, __LINE__);	\
  exit(-1);								\
}

ps_decoder_t *Init(int argc, char **argv) {
  cmd_ln_t *config;
  if ((config = cmd_ln_parse_r(NULL, ps_args(), argc, argv, TRUE)) == NULL) {
    return NULL;
  }
  ps_decoder_t *ps = ps_init(config);
  return ps;
}


// Sampling frequency.
#define FREQUENCY 16000
// Number of samples per utterance.
#define BUFFER_SAMPLES (16000 * 5)
// Number of samples to slide the window by to try and find the next utterance.
#define BUFFER_SLIDE_SAMPLES (16000 / 2)

int main(int argc, char **argv) {
  ps_decoder_t *ps = Init(argc, argv);
  CHECK(ps);

  int fd = open("goforward.raw", O_RDONLY);
  CHECK(fd >= 0);

 
  int16 buffer[BUFFER_SAMPLES];
  while (1) {
    CHECK(ps_start_utt(ps, "commandstream") >= 0);
    const char *hyp = NULL;
    int score;
    while (hyp == NULL) {
      printf(".\n");
      int bytes = read(fd, buffer, sizeof(buffer));
      CHECK(bytes >= 0);
      if (bytes == 0) {
	printf("..\n");
	sleep(1);
	continue;
      }
      CHECK(ps_process_raw(ps, buffer, bytes / 2, FALSE, FALSE) >= 0);
      const char *uttid;
      hyp = ps_get_hyp(ps, &score, &uttid);
    }
    if (score < 1000) {
      printf("LOWSCORE:*%s*, score=%d ", hyp, score);
    } else {
      printf("*%s*, score=%d ", hyp, score);
    }

    ps_end_utt(ps);
  }
  close(fd);
  ps_free(ps);
  return 0;
}
