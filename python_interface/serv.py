# -*- coding: utf-8 -*-

import hashlib
import socket
import time
import string
import sys,os
import thread
import subprocess
from subprocess import Popen, PIPE, STDOUT


def treatment(csock,qued):
    filename = sys.argv[2]
    f=open( filename, 'wb')

    md5=csock.recv(8192)
    print "md5 : '%s'"%md5
    size=csock.recv(8192)
    print "size : '%s'"%size
    received = 0
    #data = csock.recv(int(size))
    #f.write(data)
    # 2. receive data
    while received < int(size):
        print "reception"
        data = csock.recv(8192)
        received = received + len(data)
        if (not data):
            print "je break, plus de data"
            break
        f.write(data)
    print "sortie de boucle de reception"
     #3.close file
    f.close()
    print filename, "Received\n"
    f=open(filename,'r')
    fdata = f.read()
    print hashlib.md5(data).hexdigest()
    print str(md5) == str(hashlib.md5(data).hexdigest())
    print "'%s' == '%s'"%(md5,hashlib.md5(data).hexdigest())
    return

    
    os.mkdir("%s/tmp"%os.getcwd())
    os.chdir("%s/tmp"%os.getcwd())
    a = os.popen("tar xvf ../%s"%(filename))
    a.close()
    print "archive extracted"
    os.chdir("./SBCN_jm/")
    f=open("command_output","w")
    subprocess.Popen(["sh","%s/launch.sh"%os.getcwd()],stdout=f)
    while True:
        time.sleep(2)
        a=os.popen("tail -n 1 command_output")
        line = a.read()
        print line,"--------"
        csock.send(line)
        a.close()
        if len(line.split("%%"))>1 and "100 %%" in line:
            break
    f.close()
    csock.close()




sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('', int(sys.argv[1])))
sock.listen(5)

try:
    while True:
        # 1. client connect, open file
        newSocket, address = sock.accept()
        print "Connection from ", address
        thread.start_new_thread(treatment,(newSocket,None))


finally:
    sock.close()

