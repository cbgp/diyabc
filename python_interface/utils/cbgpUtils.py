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
        except Exception as e:
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
    def __init__(self,myfile,parent=None):
        self.parent=parent
        self.myfile = myfile
        self.sep = "+++"
        self.dicoDoc = {
                "nbScLabel":{"plop":"Number of scenario <a href='http://free.fr'>ploppppp</a>set in the <font color='red'>historical</font> model<table border='1'><tr><td>ploppppp</td></tr></table>"},
        #'nbScLabel' : '<ul id="master">\
        #                <li><input type="checkbox" id="users" checked><label for="users">Users</label>\
        #                   <ul>\
        #                    <li><input type="text" id="Bra"><label for="Bra"><a href="voila.fr">Brad</a></label>\
        #                   </ul>\
        #                </ul>\
        #',


                "nbParamLabel":"Number of parameters set in the historical model",
                }
        if os.path.exists(myfile):
            self.loadDocFile()
        else:
            raise Exception("The documentation file %s was not found"%myfile)

    def loadDocFile(self):
        if self.myfile.endswith(".xml"):
            self.loadXmlFile()
        else:
            self.loadHtmlFile()

    def loadHtmlFile(self):
        """ Loads the html file to fill the doc hashtable
        """
        f = open(self.myfile,'r')
        lines = f.readlines()
        f.close()

        l = 0
        while l<len(lines):
            section = ""
            # deux cas : le doc_ est juste après le <A NAME="SECTION000  ----> on trouve le num de section après
            # sinon on le trouve avant
            if '<A NAME="doc_' in lines[l]:
                key = lines[l].split('<A NAME="doc_')[1].split('"')[0]
                tags = key.split(self.sep)[1:]
                key = key.split(self.sep)[0]
                # on cherche le num de section après
                if '<A NAME="SECTION00' in lines[l-1]:
                    j=l+1
                    while '<SPAN CLASS="arabic">' not in lines[j]:
                        j+=1
                # on cherche avant
                else:
                    j=l-1
                    while '<SPAN CLASS="arabic">' not in lines[j]:
                        j-=1
                # j est sur la ligne ou se trouve le num de section
                for d in lines[j].split('<SPAN CLASS="arabic">')[1:]:
                    section += "%s."%(d.split('</SPAN>')[0])
                section = section[:-1]

                # doc_ trouvé, recup de la description
                desc = ""
                while '<A NAME="SECTION00' not in lines[l] and len(desc) < 2500:
                    if '<SPAN CLASS="arabic">' not in lines[l]:
                        desc += lines[l]
                    l+=1

                desc+="<br/><br/>More details in the documentation pdf at section : %s"%section
                self.addDescription(key,desc,tags)
            l+=1

    def addDescription(self,key,desc,tags=[]):
        # cleaning empty tags
        while tags.count("") > 0:
            tags.remove("")
        # creating entry for this object if it doesn't exist
        if not self.dicoDoc.has_key(key):
            self.dicoDoc[key] = {}
        # if no tag specified, inserting in tag : default_tag
        if tags == []:
            tags.append("default_tag")
        # puts descriptions in tags strings
        for tag in tags:
            if self.dicoDoc[key].has_key(tag):
                self.dicoDoc[key][tag] += "\n"+desc
            else:
                self.dicoDoc[key][tag] = desc

    def loadXmlFile(self):
        """ Loads the xml file to fill the doc hashtable
        """
        doc = parse(self.myfile)

        elemList = []
        for elemType in ['paragraph','section','subsection','subsubsection']:
            elemList.extend(doc.getElementsByTagName(elemType))

        for i in elemList:
            if i.hasAttribute('labels'):
                if "doc_" in i.getAttribute('labels'):
                    key = i.getAttribute('labels').replace('LABEL:doc_','')
                    tags = key.split(self.sep)[1:]
                    key = key.split(self.sep)[0]
                    value = i.getElementsByTagName('para')[0].getElementsByTagName("p")[0].childNodes[0].nodeValue
                    value+="\n\nMore details in the documentation pdf at section : "
                    value+=i.getAttribute('xml:id').replace('S','').replace('p','').replace('P','')
                    
                    self.addDescription(key,value,tags)
        #print self.dicoDoc

    def getDocString(self,key):
        """ @rtype: string
        @return: the doc string related to the key
        """
        result = None
        if key in self.dicoDoc.keys():
            result = ""
            if self.dicoDoc[key].has_key("default_tag"):
                result += self.dicoDoc[key]["default_tag"]+"\n"
            for tag in self.dicoDoc[key]:
                if tag != "default_tag":
                    result += "%s\n"%tag
                    result += self.dicoDoc[key][tag]+"\n"
        else:
            raise Exception("No documentation found for key %s"%key)
        return result

    def getDocHashByTags(self,key):
        """ @rtype: hashtable
        @return: the hashtable indexed by tags of key's descriptions
        """
        if key in self.dicoDoc.keys():
            return self.dicoDoc[key]
        else:
            raise Exception("No documentation found for key %s"%key)


from datetime import datetime 
import time

def sizedirectory(path):
    """ @rtype: int
    @return: the size of a directory in bytes

    >>> sizedirectory("/tmp")
    123456
    """
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
    except Exception as e:
        raise Exception("Log rotation failed\n%s"%e)

import array

def readRefTableSize(reftablefile):
    """ @rtype: int
    @return: the record number in binary reftable header
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
    """ @rtype: int
    @return: record number of a given scenario
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

        print("%s[%02d/%02d/%s %02d:%02d:%02d] {%s} [[%s]] %s : %s"%(color,dd.day,dd.month,dd.year,dd.hour,dd.minute,dd.second,level,func,WHITE,message))

def addLine(filepath,content):
    """ Add a content at the end of a file
    """
    f = open(filepath,'a')
    f.write(content)
    f.close()

def getRamInfo():
    """ @rtype: (int,int)
    @return: total and available RAM sizes in mega bytes
    """
    result = None
    # LINUX
    if "linux" in sys.platform:
        import re
        pat = re.compile(r'\s+')
        f = open("/proc/meminfo",'r')
        s = f.read().strip()
        f.close()
        s = s.split('\n')
        for line in s:
            cline = pat.sub(' ',line)
            if "MemTotal" in line:
                totalRam = int(cline.split(' ')[1])/1000
            elif "MemFree" in line:
                availRam = int(cline.split(' ')[1])/1000
        result = (totalRam , availRam)

    # WINDOWS
    elif "win" in sys.platform and "darwin" not in sys.platform:
        import ctypes

        kernel32 = ctypes.windll.kernel32
        c_ulong = ctypes.c_ulong
        class MEMORYSTATUS(ctypes.Structure):
            _fields_ = [
                ('dwLength', c_ulong),
                ('dwMemoryLoad', c_ulong),
                ('dwTotalPhys', c_ulong),
                ('dwAvailPhys', c_ulong),
                ('dwTotalPageFile', c_ulong),
                ('dwAvailPageFile', c_ulong),
                ('dwTotalVirtual', c_ulong),
                ('dwAvailVirtual', c_ulong)
            ]

        memoryStatus = MEMORYSTATUS()
        memoryStatus.dwLength = ctypes.sizeof(MEMORYSTATUS)
        kernel32.GlobalMemoryStatus(ctypes.byref(memoryStatus))
        mem = memoryStatus.dwTotalPhys / (1024*1024)
        availRam = memoryStatus.dwAvailPhys / (1024*1024)
        totalRam = int(mem)
        result = (totalRam , availRam)
    # MACOS
    elif "darwin" in sys.platform:
        import os
        cmd =  "echo $(( $(vm_stat | grep free | awk '{ print $3 }' | sed 's/\\.//')*4096/1048576 ))"
        p = os.popen(cmd)
        s = p.read()
        p.close()
        availRam = int(s)
        # TODO : ce n'est pas vrai mais je n'ai pas encore trouvé
        totalRam = availRam
        result = (totalRam , availRam)

    return result

from PyQt4.QtCore import *
from PyQt4.QtGui import *
def checkRam():
    """ Check if available RAM amount is at least equal to 2 GB
    A notification is showed if not.
    """
    try:
        raminfo = getRamInfo()
    except Exception as e:
        raminfo = None
        return
    min_requested_ram = 2000
    if raminfo[1] < min_requested_ram:
        #output.notify(None,"System warning","Warning, your have less than %s MB of RAM available.\n Your computer may swap and become very slow"%min_requested_ram)
        QMessageBox.information(None,"System warning","Warning, your have less than %s MB of RAM available.\n Your computer may swap and become very slow"%min_requested_ram)
