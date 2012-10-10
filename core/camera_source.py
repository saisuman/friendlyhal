import sys
import os

class FFMpegStream(object):
    DEFAULT_CAMERA_DEVICE = '/dev/video0'
    INPUT_FORMAT = 'video4linux2'
    INPUT_RESOLUTION = '640x480'
    INPUT_VIDEO_BITRATE = 1048576
    OUTPUT_VIDEO_CODEC = 'copy'
    OUTPUT_VIDEO_FORMAT = 'mpegts'

    def __init__(self, camera_device=None):
        self.camera_device = camera_device if camera_device else self.DEFAULT_CAMERA_DEVICE

    def start_stream(self):
        ffmpeg_cmd = '/usr/local/bin/ffmpeg -f %s -t %s -b:v %s -i %s -vcodec %s -b:v %s -f %s - ' % (
            self.INPUT_FORMAT,
            self.INPUT_RESOLUTION,
            self.INPUT_VIDEO_BITRATE,
            self.camera_device,
            self.OUTPUT_VIDEO_CODEC,
            self.INPUT_VIDEO_BITRATE,
            self.OUTPUT_VIDEO_FORMAT)
        print 'DEBUG: Executing ffmpeg cmd: %s' % ffmpeg_cmd
        _ignored, self.mpegts_stream, err = os.popen3(ffmpeg_cmd)
        outfile = open('/tmp/crap', 'w')
        while True:
            data = self.mpegts_stream.read(65536)
            if not data:
                print 'Finished, read %d bytees.' % len(data)
                break
            outfile.write(data)
            print 'OK: Wrote %d bytes.' % len(data)

    def 

FFMpegStream().start_stream()
