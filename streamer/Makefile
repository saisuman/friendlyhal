%.o: %.cc
	$(CC) -c $^ -o $@

video_streamer: video_streamer.o video_wrappers.o log_and_err_util.o
	$(CC) \
	video_streamer.o \
	video_wrappers.o \
	log_and_err_util.o \
	-lv4l2 -o video_streamer

clean:
	rm -f *.o video_streamer