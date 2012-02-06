#!/usr/bin/python
# -*- coding: utf-8 -*-

from PyQt4.QtCore import QThread
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import os.path,subprocess,time

class LauncherThread(QThread):
    """ classe qui gère l'execution du programme externe
    Signals description :
    log(int,QString) : log signal
    progress(QString) : progress file content changed, the string is the new content
    termSuccess() : program ended successfully
    termProblem(QString) : program ended without susccess, the string is the problem description
    """
    def __init__(self,name,cmd_args_list,outfile_path,progressfile_path=None,
            signalnames={"progress":"progress","log":"log","termSuccess":"termSuccess","termProblem":"termProblem"}):
        super(LauncherThread,self).__init__()
        self.name = name
        self.cmd_args_list = cmd_args_list
        self.outfile_path = outfile_path
        self.progressfile_path = progressfile_path
        self.progress_file_content = ""
        self.processus = None
        try:
            self.SIGPROGRESS = signalnames["progress"] 
            self.SIGLOG = signalnames["log"]
            self.SIGTERMSUCCESS = signalnames["termSuccess"]
            self.SIGTERMPROBLEM = signalnames["termProblem"]
        except Exception as e:
            print e
            raise Exception("Signal name hashtable malformed")

    def log(self,lvl,msg):
        """ evite de manipuler les objets Qt dans un thread non principal
        """
        clean_msg = msg.replace(u'\xb5',u'u')
        self.emit(SIGNAL(self.SIGLOG+"(int,QString)"),lvl,clean_msg)

    def killProcess(self):
        if self.processus != None:
            if self.processus.poll() == None:
                self.log(3,"Killing process (pid:%s) of thread %s"%(self.processus.pid,self.name))
                self.processus.kill()

    def readProgress(self):
        """ read progress file and emit a signal if its content has changed
        """
        if os.path.exists(self.progressfile_path):
            a = open(self.progressfile_path,"r")
            content = a.read()
            a.close()
            # we progressed !
            if content != self.progress_file_content:
                self.progress_file_content = content
                self.emit(SIGNAL(self.SIGPROGRESS+"(QString)"),content)
            else:
                self.log(3,"Progress file of thread '%s' didn't change"%self.name)
        else:
            self.log(3,"Progress file '%s' not found"%self.progressfile_path)

    def readProblem(self):
        """ read progress file and returns its first line
        """
        problem = ""
        if os.path.exists(self.progressfile_path):
            a = open(self.progressfile_path,"r")
            lines = a.readlines()
            a.close()
            if len(lines) > 0:
                problem = lines[0]
        return problem

    def run(self):
        self.log(2,"Running '%s' thread execution"%self.name)
        f = open(self.outfile_path,"w")
        p = subprocess.Popen(self.cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT) 
        self.log(3,"Command launched in thread '%s' : %s"%(self.name," ".join(self.cmd_args_list)))
        self.processus = p
        self.log(3,"Popen procedure success")
        outlastline = ""
        while True:
            time.sleep(2)
            if self.progressfile_path != None:
                self.readProgress()
            poll_check = p.poll()
            if poll_check != None:
                f.close()
                f = open(self.outfile_path,"r")
                lines = f.readlines()
                f.close()
                if len(lines) > 0:
                    outlastline = lines[-1]
                    # limitating length of last output line
                    if len(outlastline) > 300:
                        outlastline = outlastline[-300:]
                # success
                if poll_check == 0:
                    self.emit(SIGNAL(self.SIGTERMSUCCESS+"()"))
                # failure
                else:
                    if self.progressfile_path != None:
                        time.sleep(2)
                        redProblem = self.readProblem()
                        problem = "Program of thread '%s' exited (with return code %s) unsuccessfully.\n%s\n\n%s"%(self.name,poll_check,outlastline,redProblem)
                    else:
                        problem = "Program of thread '%s' exited (with return code %s) unsuccessfully.\n%s"%(self.name,poll_check,outlastline)
                    self.emit(SIGNAL(self.SIGTERMPROBLEM+"(QString)"),problem)
                return

