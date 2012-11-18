# Servlet for returning video streams in response
# to GET requests.
from mod_python import apache

from core import camera_source

def get(req):
	req.content_type = 'video/MP2T'
	f = open('/tmp/out.mpeg')
	while True:
		d = f.read()
		req.write(d)
		if not d:
			break

