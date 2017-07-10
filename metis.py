boards=['Metis','Hermes','Griffin','?','Angelia','Orion','Hermes_Lite']

def decode_hwtopc(data,addr):
    l = len(data)
    #check header
    if ord(data[0]) != 0xEF and ord(data[1]) != 0xFE:
        print "bad header"
        return

    cmd = ord(data[2])

    mac="%02X:%02X:%02X:%02X:%02X:%02X" % ( ord(data[3]), ord(data[4]), ord(data[5]), ord(data[6]), ord(data[7]), ord(data[8]))
    version=ord(data[9])
    board=ord(data[10])
    try:
      boardname=boards[board]
    except e:
      boardname="Unknown"

    if cmd==2:
      status="Stopped"
    elif cmd==3:
      status="Sending"
    else:
      status="Unknown"

    print addr[0],"[",mac,"]", boardname, "version", version,"Status:",status

