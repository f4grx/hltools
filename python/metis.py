boards=['Metis','Hermes','Griffin','?','Angelia','Orion','Hermes_Lite']

def decode_hwtopc(data,addr):
    l = len(data)
    #check header
    if data[0] != 0xEF and data[1] != 0xFE:
        print("bad header")
        return

    cmd = data[2]

    mac="%02X:%02X:%02X:%02X:%02X:%02X" % ( data[3], data[4], data[5], data[6], data[7], data[8])
    version=data[9]
    board  =data[10]
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

    print(addr[0],"[",mac,"]", boardname, "version", version,"Status:",status)

