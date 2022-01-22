#!/usr/bin/env python3


from socket import *
import datetime

soc = socket(AF_INET, SOCK_DGRAM)
soc.bind(('', 1338))

while True:	
	data, addr = soc.recvfrom(2048) # buffer size is 1024 bytes
	print("%s" % data)

