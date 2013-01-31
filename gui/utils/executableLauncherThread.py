#!/usr/bin/python
# -*- coding: utf-8 -*-

from PyQt4.QtCore import QThread
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import os.path,subprocess,time,sys

class LauncherThread(QThread):
    """ Manage the execution of an external executable and
    communicate informations by QT Signals

    Can handle :
    - progress file scrutation
    - output reading
    - logging
    - process killing
    - errors transmission

    Signals description :
    log(int,QString) : log signal
    progress(QString) : progress file content changed, the string is the new content
    termSuccess() : program ended successfully
    termProblem(QString) : program ended without susccess, the string is the problem description

    Here are the two classic scenarios to use this class :
    - periodical scrutation : 
        every 2 seconds, check if the executable is still running and if the progress file has
        changed. 
    - real time scrutation : 
        every line produced by the executable on its stdout is sent by a signal in real time
    """
    def __init__(self,name,cmd_args_list,realtime_output=False,outfile_path=None,progressfile_path=None,
            signalnames={"progress":"progress","log":"log","termSuccess":"termSuccess","termProblem":"termProblem","newOutput":"newOutput"}):
        """ If progressfile_path is None or not given, no progress scrutation will be performed
        If signalnames is not given, signals will have basic names
        """
        super(LauncherThread,self).__init__()
        self.name = name
        self.cmd_args_list = cmd_args_list
        self.cmd_args_list_quoted = list(self.cmd_args_list)
        for i in range(len(self.cmd_args_list_quoted)):
            if ";" in self.cmd_args_list_quoted[i] or " " in self.cmd_args_list_quoted[i] or ":" in self.cmd_args_list_quoted[i]:
                self.cmd_args_list_quoted[i] = '"'+self.cmd_args_list_quoted[i]+'"'

        self.realtime_output = realtime_output
        self.outfile_path = outfile_path
        self.progressfile_path = progressfile_path
        self.progress_file_content = ""
        self.processus = None
        try:
            self.SIGPROGRESS = signalnames["progress"]
            self.SIGLOG = signalnames["log"]
            self.SIGTERMSUCCESS = signalnames["termSuccess"]
            self.SIGTERMPROBLEM = signalnames["termProblem"]
            self.SIGNEWOUTPUT = signalnames["newOutput"]
        except Exception as e:
            print e
            raise Exception("Signal name hashtable malformed")

    def log(self,lvl,msg):
        """ Send a log signal with level and content
        Avoid manipulation of Qt objects in a thread which is not the Qt main loop
        @param lvl: Importance level of the log
        @param msg: Content of the log
        """
        clean_msg = msg.replace(u'\xb5',u'u')
        self.emit(SIGNAL(self.SIGLOG+"(int,QString)"),lvl,clean_msg)

    def killProcess(self):
        """ Terminate the processus of the external executable if it is running
        """
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
        """ Threaded execution, launch the executable, then loop on :
        check the progression
        read the output
        check if the external program is still running
        send success or problem signal
        """
        self.log(2,"Running '%s' thread execution"%self.name)
        # realtime output watch mode
        if self.realtime_output:
            self.realtimeOutputWatch()
        else:
            self.intervalWatch()

    def realtimeOutputWatch(self):
        self.log(2,"Beginning realtime watch")
        if "win" in sys.platform and "darwin" not in sys.platform:
            popen_options = subprocess.STARTUPINFO()
            popen_options.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            p = subprocess.Popen(self.cmd_args_list,startupinfo=popen_options, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.PIPE)
        else:
            p = subprocess.Popen(self.cmd_args_list, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.PIPE)
        self.processus = p
        for line in iter(p.stdout.readline, ""):
            self.emit(SIGNAL(self.SIGNEWOUTPUT+"(QString)"),line)

        poll_check = p.poll()
        if poll_check == 0:
            self.emit(SIGNAL(self.SIGTERMSUCCESS+"()"))
        else:
            problem = "Program of thread '%s' exited (with return code %s) unsuccessfully."%(self.name,poll_check)
            self.emit(SIGNAL(self.SIGTERMPROBLEM+"(QString)"),problem)

    def intervalWatch(self):
        self.log(2,"Beginning interval watch")
        f = open(self.outfile_path,"w")
        if "win" in sys.platform and "darwin" not in sys.platform:
            popen_options = subprocess.STARTUPINFO()
            popen_options.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            p = subprocess.Popen(self.cmd_args_list,startupinfo=popen_options, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
        else:
            p = subprocess.Popen(self.cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)

        self.log(3,"Command launched in thread '%s' : %s"%(self.name," ".join(self.cmd_args_list_quoted)))
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
                time.sleep(1)
                f = open(self.outfile_path,"r")
                lines = f.readlines()
                f.close()
                if len(lines) > 4:
                    outlastline = "\n".join(lines[-5:-1])
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
                        problem = "Program of thread '%s' exited (with return code %s) unsuccessfully.\n\n%s\n\nContent of progress file :\n\n%s"%(self.name,poll_check,outlastline,redProblem)
                    else:
                        problem = "Program of thread '%s' exited (with return code %s) unsuccessfully.\n\n%s"%(self.name,poll_check,outlastline)
                    self.emit(SIGNAL(self.SIGTERMPROBLEM+"(QString)"),problem)
                return

