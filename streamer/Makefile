CC=c++
CFLAGS=-O3
%.o: %.cc
	$(CC) $(CFLAGS) -c $^ -o $@

video_streamer: video_streamer.o video_wrappers.o
	$(CC) $(CFLAGS) \
	video_streamer.o \
	video_wrappers.o \
	-lglog -lv4l2 -o video_streamer

clean:
	rm -f *.o video_streamer
