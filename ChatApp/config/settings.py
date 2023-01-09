# -*- coding: utf-8 -*-
import multiprocessing


name = 'server.py'

# Process Naming
proc_name = 'gunicorn'

# Logging
logfile = './logs/{}.log'.format(name)
loglevel = 'info'
logconfig = None

# Server Socket
bind = '0.0.0.0:8080'

# Worker Processes ... pip3 install gevent
worker_class = 'gevent'
# workers = multiprocessing.cpu_count() * 2 + 1
workers = 1
worker_connections = 1024
max_requests = 10000


timeout = 120
keepalive = 2

backlog = 2048


# Server Mechanics
preload_app = True
debug = True
reload = debug
daemon = True
pidfile = 'gunicorn.pid'

daemon = False

secure_scheme_headers = {
     'X-FORWARDED-PROTOCOL': 'ssl',
     'X-FORWARDED-PROTO':    'https',
     'X-FORWARDED-SSL':      'on',
 }