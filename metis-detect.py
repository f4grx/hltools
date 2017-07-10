#!/usr/bin/env python
#this program discovers all hermes/hermes lite boards on the local net
#https://wiki.python.org/moin/UdpCommunication
#https://docs.python.org/2/library/socket.html

#attemt to discover Hermes / Openhpsdr boards using protocol v1

import socket,metis


BCAST="255.255.255.255"
DEST_PORT=1024
DISC="\xEF\xFE\x02" + (60 * "\x00")

print "init socket"
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1);
sock.bind(('',0))

print "send discovery"
sock.sendto(DISC, (BCAST, DEST_PORT))

sock.settimeout(1)
while True:
  try:
    data,addr = sock.recvfrom(1024)
    metis.decode_hwtopc(data,addr)
  except Exception as e:
    print "error ->",e
    break

print "done"
sock.close()
