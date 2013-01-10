# We're going to run the mjpg streamer on a different port from the apache server.
# We'll use apache reverse proxying to serve the video feed through the same server
# and use apache authentication. But that leaves the mjpg streamer unguarded on
# port 8080, and you can't get it to bind to a single interface. So we simply
# drop all incoming on 8080.
iptables -A INPUT -p tcp --destination-port 8080 -i wlan0 -d 192.168.1.119 -j DROP
# Need to add to LD_LIBRARY_PATH so that the mjpg_streamer libs can be picked up.
export LD_LIBRARY_PATH=/usr/local/lib
# For the rest of the python code.
export PYTHONPATH=/home/suman/dev/friendlyhal/core
export PATH=/home/suman/dev/friendlyhal/core
echo "Killing an existing one first . . ."
killall mjpg_streamer
killall controller.py
echo "Starting streamer . . . "
/usr/local/bin/mjpg_streamer -i "libinput_uvc.so -d /dev/video3" -o "liboutput_http.so" &
echo "Starting controller listener . . ."
# The status LEDs are too close to the passive IR motion sensor and keep tripping
# it. So turn them off.
echo 0 > /sys/class/leds/pandaboard::status1/brightness
echo 0 > /sys/class/leds/pandaboard::status2/brightness
controller.py
