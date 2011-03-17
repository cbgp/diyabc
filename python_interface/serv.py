# -*- coding: utf-8 -*-

import datetime
from datetime import *
import hashlib
import socket
import time
import string
import sys,os
import thread
import subprocess
from subprocess import Popen, PIPE, STDOUT


def treatment(csock,qued):
    #filename = sys.argv[2]
    filename = str(datetime.now())
    filename = filename.replace(' ','_').replace(":",".")
    filename+=".tar"
    f=open( filename, 'wb')

    md5=csock.recv(33)
    md5 = str(md5).strip()
    print "md5 : '%s'"%md5
    size=csock.recv(20)
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
    f.close()
    print str(hashlib.md5(fdata).hexdigest())
    checkfile = (md5 == str(hashlib.md5(fdata).hexdigest()))
    print "'%s' == '%s'"%(md5,hashlib.md5(fdata).hexdigest())

    if checkfile:
        dirname = filename.replace('.tar','')
        os.mkdir("%s/%s"%(os.getcwd(),dirname))
        #os.chdir("%s/%s"%(os.getcwd(),dirname))
        a = os.popen("tar xvf %s -C ""%s"""%(filename,dirname))
        a.close()
        print "archive extracted"
        #os.chdir("./SBCN_jm/")
        f=open("%s/SBCN_jm/command_output"%dirname,"w")
        subprocess.Popen(["sh","./launch.sh"],stdout=f,cwd="%s/SBCN_jm/"%dirname)
        while True:
            time.sleep(2)
            a=os.popen("tail -n 1 %s/SBCN_jm/command_output"%dirname)
            line = a.read()
            print "[",dirname,"] ",line,"--------"
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

