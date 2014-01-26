import BaseHTTPServer
import urlparse

STATIC_FILES = {
    'console.html': 'text/html',
    'style.css': 'text/css',
    'console.js': 'text/javascript',
    'calendar.html': 'text/html',
    'calendar.js': 'text/html',
    '/': ('console.html', 'text/html'),
}

class ConsoleRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def __init__(self):
        BaseHTTPRequestHandler.BaseHTTPRequestHandler.__init__(self)

    def do_GET(self):
        parsed_path = urlparse.urlparse(self.path)
        self.send_response(200)
        self.end_headers()
        self.wfile.write('OK')

def init_webserver(server_class=BaseHTTPServer.HTTPServer,
        handler_class=ConsoleRequestHandler):
    server_address = ('', 8000)
    httpd = server_class(server_address, handler_class)
    return httpd

def main():
    server = init_webserver()
    server.serve_forever()

if __name__ == '__main__':
    main()
