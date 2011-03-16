# -*- coding: utf-8 -*-

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

    size=csock.recv(8192)
    print "size : %s"%size
    #data = csock.recv(int(size))
    #f.write(data)
    # 2. receive data
    while True:
        data = csock.recv(8192)
        if (not data):
            print "je break"
            break
        f.write(data)
        if str(os.path.getsize(filename)) == str(size):
            break
     #3.close file
    f.close()
    print filename, "Received\n"
    
    os.mkdir("%s/tmp"%os.getcwd())
    os.chdir("%s/tmp"%os.getcwd())
    a = os.popen("tar xvf ../%s"%(filename))
    a.close()
    print "archive extracted"
    os.chdir("./SBCN_jm/")
    f=open("command_output","w")
    subprocess.Popen(["sh","%s/launch.sh"%os.getcwd()],stdout=f)
    while True:
        time.sleep(3)
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

