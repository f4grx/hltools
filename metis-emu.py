#!/usr/bin/env python
#this program emulates a hpsdr board

import socket, errno

CMD_DISCOVER = 0x02

STATUS_STOPPED = 0x02
STATUS_RUNNING = 0x03

BOARD_ID_METIS      = 0x00
BOARD_ID_HERMES     = 0x01
BOARD_ID_GRIFFIN    = 0x02
BOARD_ID_ANGELIA    = 0x04
BOARD_ID_ORION      = 0x05
BOARD_ID_HERMESLITE = 0x06

#reported info
mac = "\x1c\x1b\x0d\x11\x4e\x88"
version = 1
board = BOARD_ID_HERMESLITE
#current status
status = STATUS_STOPPED

def decode(data,addr):
    l = len(data)
    print l, "bytes from",addr,":",data
    #check header
    if ord(data[0]) != 0xEF and ord(data[1]) != 0xFE:
        print "bad header"
        return
        
    cmd = ord(data[2])
    print "cmd:",cmd
    if cmd == CMD_DISCOVER:
        print "discover"
        #make reply
        reply = "\xef\xfe"+chr(status)+mac+chr(version)+chr(board)
        sock.sendto(reply, addr)
        print "reply sent to",addr
    else:
        print "unknown command"
        
print "init socket"
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1);
sock.bind(('0.0.0.0', 1024))
sock.setblocking(False)

print "waiting data"
while True:
    try:
        data,addr = sock.recvfrom(1024)
    except socket.error as e:
        if e.errno == errno.EAGAIN or e.errno == 10035:
            continue
        else:
            print "error:",e
            sock.close()
            break
        
    decode(data,addr)
