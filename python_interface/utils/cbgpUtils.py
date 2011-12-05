# -*- coding: utf-8 -*-

## @package python_interface.utils.cbgpUtils
# @author Julien Veyssier
#
# @brief Tools helping all kinds of progammers
#
# Package regrouping generic tools created for DIYABC interface
# but usefull for any graphical or user oriented program.
# 
# This package contains several classes : 
#     - CmdPrompt and cmdThread : prompt which allows to execute python code
#       interactively while program is running
#     - Documentator : Extract infos from xml files produced by latexml to get
#       documentation of paragraphs* labeled like "doc_*"
#     - TeeLogger : Replace a standard output in order to redirect the flows to :
#       a file, the replaced output and an external function
# 
# And several functions :
#     - logRotate : recursively delete old files if the size of a directory
#       exceeds a threshold
#     - readRefTableSize : extract the record number of a DIYABC binary reference
#       table 
#     - readNbRecordsOfScenario : extract the record number which concern a
#       specific scenario
#     - log : print the given string decorated by the date, the log level and the
#       context (caller of calling function, calling function, parameters of
#       calling function).
# 

from threading import Thread
import sys
import cmd
import inspect
import re

LOG_LEVEL = 4

RED='\033[31m'
WHITE='\033[00m'
BLUE='\033[35m'
GREEN='\033[32m'
YELLOW='\033[33m'
CYAN='\033[36m'
CYAN_BLINK='\033[5;36m'
tabcolors=[WHITE,RED,BLUE,GREEN,YELLOW,CYAN,CYAN_BLINK]

class CmdPrompt(cmd.Cmd):
    """ prompt which execute python code given by the user
    with the "do" command
    """
    def do_do(self,line):
        try:
            exec line
        except Exception,e:
            print e
    
    def do_EOF(self, line):
        return True

class cmdThread(Thread):
    """ Command prompt launch thread
    """
    def __init__(self,diy):
        super(cmdThread,self).__init__()
        self.diy = diy
        self.start()
    def run(self):
        plop = CmdPrompt()
        plop.diy=self.diy
        plop.cmdloop()


from xml.dom.minidom import parse
import os.path

class Documentator():
    """ Class which extracts internal doc by parsing an xml file
    generated from latex sources of DIYABC instructions paper. This class also provide
    access primitives to this doc from keywords (qt objectnames in our case)
    """
    def __init__(self,xmlFile,parent=None):
        self.parent=parent
        self.xmlFile = xmlFile
        self.dicoDoc = {
                "nbScLabel":"Number of scenario set in the historical model",
                "nbParamLabel":"Number of parameters set in the historical model",
                }
        if os.path.exists(xmlFile):
            self.loadDocFile()
        else:
            raise Exception("The xml documentation file %s was not found"%xmlFile)

    def loadDocFile(self):
        """ Loads the xml file to fill the doc hashtable
        """
        doc = parse(self.xmlFile)

        elemList = []
        for elemType in ['paragraph','section','subsection','subsubsection']:
            elemList.extend(doc.getElementsByTagName(elemType))

        for i in elemList:
            if i.hasAttribute('labels'):
                if "doc_" in i.getAttribute('labels'):
                    key = i.getAttribute('labels').replace('LABEL:doc_','')
                    value = i.getElementsByTagName('para')[0].getElementsByTagName("p")[0].childNodes[0].nodeValue
                    value+="\n\nMore details in the documentation pdf at section : "
                    value+=i.getAttribute('xml:id').replace('S','').replace('p','').replace('P','')
                    self.dicoDoc[key] = value

    def getDocString(self,key):
        """ Return the doc related to the key
        """
        result = None
        if key in self.dicoDoc.keys():
            result =  self.dicoDoc[key]
        else:
            raise Exception("No documentation found for key %s"%key)
        return result


from datetime import datetime 
import time

def sizedirectory(path):
    size = 0
    for root, dirs, files in os.walk(path):
        for fic in files:
            size += os.path.getsize(os.path.join(root, fic))
    return size


def logRotate(logFolder,nbDaysOld,sizeThreshold):
    """ Clean logFolder directory of all files older than nbDaysOld 
    only if logFolder is bigger than sizeThreshold Mo
    """
    try:
        logSize = sizedirectory(logFolder)
        if (logSize / (1024*1024)) > sizeThreshold:
            log(2,"Logrotate process launched")
            ddNow = datetime.now()
            for root,dirs,files in os.walk(os.path.expanduser("~/.diyabc/logs/")):
                for name in files:
                    dateLastModif = time.gmtime(os.stat(os.path.join(root, name)).st_mtime)
                    ddMod = datetime(dateLastModif.tm_year,dateLastModif.tm_mon,dateLastModif.tm_mday)
                    delta = ddNow - ddMod
                    if delta.days > nbDaysOld :
                        log(4,"Deleting %s because it is %s days old"%(os.path.join(root, name),delta.days))
                        os.remove(os.path.join(root, name))

        logSize = sizedirectory(logFolder)
        # if the size of the folder is still bigger than the threshold
        # another logrotate is done on older files
        if (logSize / (1024*1024)) > sizeThreshold:
            logRotate(logFolder,nbDaysOld - 2, sizeThreshold)
    except Exception,e:
        raise Exception("Log rotation failed\n%s"%e)

import array

def readRefTableSize(reftablefile):
    """ Read the record number in binary reftable header
    """
    if os.path.exists(reftablefile):
        binint = array.array('i')
        f = open(reftablefile,'rb')
        binint.read(f,1)
        f.close()
        rtSize = binint[0]
        return rtSize
    else:
        return None

def readNbRecordsOfScenario(reftablefile,numSc):
    """ Read record number of a given scenario
    in binary reftable header
    """
    if os.path.exists(reftablefile):
        binint = array.array('i')
        f = open(reftablefile,'rb')
        binint.read(f,numSc+2)
        f.close()
        nbRec = binint[-1]
        return nbRec
    else:
        return None

class TeeLogger(object):
    """ Replacement object to stdout or stderr to log output on stdout, a log file
    and by calling an external log function.
    WARNING : A side effect is that stdout and stderr are not identifiable in
    the log file and in the external log function.
    The external log function must have this prototype :
    showExternal("string to display")
    """
    def __init__(self, name, out_or_err, showExternal=None):
        """ replace stdout or stderr
        """
        self.showExternal = showExternal
        #self.logRotate(name)

        #self.file = open(name, mode)
        self.filename = name 
        self.out_or_err = out_or_err
        # on sauvegarde le out que l'on remplace
        if out_or_err:
            self.out = sys.stdout
        else:
            self.out = sys.stderr
    def __del__(self):
        pass
        ## genere une exception que je ne comprends pas
        # Exception AttributeError: "'NoneType' object has no attribute 'stdout'" in <bound method TeeLogger.__del__ of <output.TeeLogger object at 0x92ce5ec>> ignored
        #if self.out_or_err:
        #    sys.stdout = self.out
        #else:
        #    sys.stderr = self.out

        #self.file.close()
    def write(self, data):
        #data = data.replace(u'\xb5','u')
        data_without_color = data.replace(RED,'').replace(WHITE,'').replace(GREEN,'').replace(BLUE,'').replace(YELLOW,'').replace(CYAN,'').replace(CYAN_BLINK,'')
        pattern = re.compile(r' \[\[.*\]\]')
        data_short = pattern.sub('',data)
        # on bouge le curseur au début de la dernière ligne
        #self.app.showLogFile_win.logText.moveCursor(QTextCursor.End)
        #self.app.showLogFile_win.logText.moveCursor(QTextCursor.StartOfLine)
        #self.app.showLogFile_win.logText.appendPlainText("%s"%pattern.sub('',data_without_color.strip()))
        if self.showExternal != None:
            self.showExternal("%s"%pattern.sub('',data_without_color.strip()))

        ftmp = open(self.filename,'a')
        ftmpdata = data_without_color
        ftmp.write(ftmpdata)
        #ftmp.file.flush()
        ftmp.close()
        self.out.write(data_short)

def log(level,message):
    """ Print the message after fancyfying it
    """
    if level <= LOG_LEVEL:
        dd = datetime.now()
        color = tabcolors[level]

        lastFrame = inspect.stack()[1][0]
        params = lastFrame.f_locals

        params = inspect.currentframe().f_back.f_locals

        func = ""
        if len(inspect.stack()) > 3:
            func = "%s >> %s(%s)"%(inspect.stack()[3][3],inspect.stack()[2][3],params)
        elif len(inspect.stack()) > 2 and len(inspect.stack()[2]) > 3:
            func = "%s(%s)"%(inspect.stack()[2][3],params)

        print "%s[%02d/%02d/%s %02d:%02d:%02d] {%s} [[%s]] %s : %s"%(color,dd.day,dd.month,dd.year,dd.hour,dd.minute,dd.second,level,func,WHITE,message)
