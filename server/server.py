#!/usr/bin/env python
from http.server import HTTPServer, CGIHTTPRequestHandler

import cgitb; cgitb.enable()

server_address = ("", 8000)  # ou 8100 pour le 2e binôme
handler = CGIHTTPRequestHandler
handler.cgi_directories = ["/cgi-bin"]

httpd = HTTPServer(server_address, handler)
print(f"Serveur HTTP lancé sur le port {server_address[1]}")
httpd.serve_forever()
