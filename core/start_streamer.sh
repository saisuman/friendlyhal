export LD_LIBRARY_PATH=/usr/local/lib
mjpg_streamer -i "libinput_uvc.so -d /dev/video3" -o liboutput_http.so
