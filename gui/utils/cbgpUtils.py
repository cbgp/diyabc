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
#     - GetFirstParentAttr : retrieve attributes values in pyQt objects chained by the parent attr
#     - CmdPrompt and cmdThread : prompt which allows to execute python code
#       interactively while program is running
#     - Documentator : Extract infos from xml files produced by latexml or latex2html
#       to get documentation of paragraphs* labeled like "doc_*"
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
import locale
import cmd
import inspect
import re
import codecs

LOG_LEVEL = 4

RED='\033[31m'
WHITE='\033[00m'
BLUE='\033[35m'
GREEN='\033[32m'
YELLOW='\033[33m'
CYAN='\033[36m'
CYAN_BLINK='\033[5;36m'
tabcolors=[WHITE,RED,BLUE,GREEN,YELLOW,CYAN,CYAN_BLINK]



class Parents() :
    """
    Retrieve attribute values for chained objects with 'parent' attribute for heredity.
    ex :
    a.v = 1
    b.parent = a
    c.parent = b
    d.parent = c
    d.parents = getFirstParentAttr(d.parent)
    d.parents.v ->  1
    d.parents.getParents = [c,b,a]
    """

    def __init__(self, parent):
        self.parent = parent

    def __getattr__(self, attr):
        parent = self.parent
        parents = []
        #while  getattr(parent, attr, "diyabcParentTest") == "diyabcParentTest" :
        while True :
            parents.append(parent.__class__.__name__)
            try :
                return getattr(parent, attr)
            except (AttributeError, TypeError) as e:
                pass
            try:
                parent = parent.parent
            except (AttributeError, TypeError) as e:
                msg, = e.args
                msg += "\nCould not found property %s in classes : %s.\nLast tried class %s has no parent class." % \
                                                           (attr, ", ".join(parents), parents[-1].__class__.__name__)
                e.args = (msg,)
                raise e

                #print e , "====", parent.__class__.__name__ ,"====", e.args
        #return getattr(parent, attr)

    def getFirstMatch(self,attrstr, *arguments, **options):
        attrstr = str(attrstr)
        parent = self.parent
        parents = []
        val = False
        method = False
        isMethod = False
        if attrstr[0] == '.' :
           attrstr =  attrstr[1:]
        if attrstr[-1] == ')' and attrstr[-2] != '(' :
            err = ValueError()
            msg = "if attrstr is a function to call, you must give the function parameters in the getFirstMatch paremeters.\n"
            msg += "ie :  parents.getFirstMatch('goto(house, how=byCar)') must be written parents.('goto()', house, how=byCar)\n"
            msg += "ie : maybe like this parents.getFirstMatch('%s()', %s)" % (  "(".join(attrstr.split('(')[:-1])   , attrstr.split('(')[-1][:-1]  )
            err.args = (msg,)
            raise err
        if len(arguments) > 0 or len(options) > 0 or attrstr[-2:] == '()' :
            isMethod = True
            if attrstr[-2:] == '()' :
                attr = attrstr[:-2]
            else :
                attr = attrstr
        else :
            attr = attrstr
        while True :
            parents.append(parent.__class__.__name__)
            try :
                exec("val = parent.%s" % attr)
                break
            except  (AttributeError, TypeError) as e:
                pass
            try:
                parent = parent.parent
            except (AttributeError, TypeError) as e:
                err = AttributeError()
                err.args = ("Could not found '%s' in any of those object classes : %s" % (attr,parents),)
                raise err
        if isMethod :
            val = val(*arguments, **options)
        return val

    def getParents(self):
        parent = self.parent
        parents = []
        while True :
            parents.append(parent.__class__.__name__)
            try:
                parent = parent.parent
            except (AttributeError, TypeError) as e:
                break
        return parents

    def getParentsNames(self):
        return [x.__class__.__name__ for x in self.getParents()]

    def __repr__(self):
        return self.getParentsNames().__repr__()

    def __str__(self):
        return self.__repr__()



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
    A reference to an external object can be given, it will be given to the prompt
    and accessible by "self.obj" in the commands
    """
    def __init__(self,obj):
        super(cmdThread,self).__init__()
        self.obj = obj
        self.start()
    def run(self):
        theCmd = CmdPrompt()
        theCmd.obj = self.obj
        theCmd.cmdloop()




from xml.dom.minidom import parse
import os.path

class Documentator():
    """ Class which extracts internal doc by parsing an xml or html file
    generated from latex sources of a notice-like paper. This class also provide
    access primitives to this doc from keywords (qt objectnames in our case)
    """
    def __init__(self,myfile,maxDescriptionLength=2500,separator="+++",parent=None):
        """
        @param myfile: Path of the html/xml documentation file
        @param maxDescriptionLength: Maximum number of characters to take for one description
        @param separator: string which separates tag names in label strings : doc_myObject+++tag_1+++tag_2
        """
        self.parent = parent
        self.myfile = myfile
        self.maxDescriptionLength = maxDescriptionLength
        self.separator = separator
        self.fsCoding = getFsEncoding(logLevel=False)
        self.dicoDoc = {}
        if os.path.exists(myfile):
            self.loadDocFile()
        else:
            raise Exception("The documentation file %s was not found"%myfile)

    def loadDocFile(self):
        """ Determine if the file is an xml or an html documentation
        in order to load it
        """
        if self.myfile.endswith(".xml"):
            self.loadXmlFile()
        else:
            self.loadHtmlFile()

    def loadHtmlFile(self):
        """ Loads the html file to fill the doc hashtable.
        The file is parsed line by line without any use of specific parser
        """
        f = open(self.myfile.encode(getFsEncoding(logLevel=False)),'rU')
        lines = f.readlines()
        f.close()

        # searching for a redefinition of the tag separator
        for line in lines:
            if '<A NAME="setTagSeparator_' in line:
                self.separator = line.split("setTagSeparator_")[1].split('"')[0]
                break

        l = 0
        while l<len(lines):
            section = ""
            # two cases :
            # - the label (doc_) is just after the section delimiter (<A NAME="SECTION000  ---->) :
            #   we find the section number after the label
            # - otherwise we find the section number before the label
            if '<A NAME="doc_' in lines[l]:
                key = lines[l].split('<A NAME="doc_')[1].split('"')[0]
                tags = key.split(self.separator)[1:]
                key = key.split(self.separator)[0]
                # looking for section number after the label
                if '<A NAME="SECTION00' in lines[l-1]:
                    j=l+1
                    while '<SPAN CLASS="arabic">' not in lines[j]:
                        j+=1
                # looking for section number before the label
                else:
                    j=l-1
                    while '<SPAN CLASS="arabic">' not in lines[j]:
                        j-=1
                # lines[j] is now the section number line
                for d in lines[j].split('<SPAN CLASS="arabic">')[1:]:
                    section += "%s."%(d.split('</SPAN>')[0])
                section = section[:-1]

                # getting the description after the label line
                desc = ""
                while '<A NAME="SECTION00' not in lines[l] and len(desc) < self.maxDescriptionLength:
                    if '<SPAN CLASS="arabic">' not in lines[l]:
                        desc += lines[l]
                    l+=1

                desc+="<br/><br/>More details in the documentation pdf at section : %s"%section
                self.addDescription(key,desc,tags)
            l+=1

    def addDescription(self,key,desc,tags=[]):
        """ Add a description for a given key
        @param key: name of the "description related" object
        @param desc: description string
        @param tags: list of tags for this description
        """
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

        # searching for a redefinition of the tag separator
        for i in elemList:
            if i.hasAttribute('labels'):
                if "setSeparator_" in i.getAttribute('labels'):
                    self.separator = i.getAttribute('labels').split("setSeparator_")[1]
                    break

        for i in elemList:
            if i.hasAttribute('labels'):
                if "doc_" in i.getAttribute('labels'):
                    key = i.getAttribute('labels').replace('LABEL:doc_','')
                    tags = key.split(self.separator)[1:]
                    key = key.split(self.separator)[0]
                    value = i.getElementsByTagName('para')[0].getElementsByTagName("p")[0].childNodes[0].nodeValue
                    value = value[:self.maxDescriptionLength]
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
    for root, dirs, files in os.walk(path.encode(getFsEncoding(logLevel=False))):
        for fic in files:
            size += os.path.getsize((os.path.join(root, fic)).encode(getFsEncoding(logLevel=False)))
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
            for root,dirs,files in os.walk(os.path.expanduser("~/.diyabc/logs/".encode(getFsEncoding(logLevel=False)))):
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

        ftmp = open(self.filename.encode(getFsEncoding(logLevel=False)),'a')
        ftmpdata = data_without_color
        try:
            ftmp.write(ftmpdata)
        except UnicodeEncodeError as e:
            log(3,"Impossible to write bad encoded string in log file")
        #ftmp.file.flush()
        ftmp.close()
        self.out.write(data_short)

def log(level,message):
    """ Print the message after fancyfying it
    """
    if level not in [None, False] and level <= LOG_LEVEL:
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
    f = open(filepath.encode(getFsEncoding(logLevel=False)),'a')
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
        f = open(u"/proc/meminfo".encode(getFsEncoding(logLevel=False)),'r')
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
        cmd =  u"echo $(( $(vm_stat | grep free | awk '{ print $3 }' | sed 's/\\.//')*4096/1048576 ))"
        p = os.popen(cmd.encode(getFsEncoding(logLevel=False)))
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
    """ Check if total RAM amount is at least equal to 2 GB
    A notification is showed if not.
    """
    try:
        raminfo = getRamInfo()
    except Exception as e:
        raminfo = None
        return
    min_requested_ram = 2000
    if raminfo[0] < min_requested_ram:
        #output.notify(None,"System warning","Warning, your have less than %s MB of RAM available.\n Your computer may swap and become very slow"%min_requested_ram)
        QMessageBox.information(None,"System warning","Warning, your system has less than %s MB of RAM.\n Your computer may swap and become very slow"%min_requested_ram)

import subprocess
m2m={
        "jan":"01",
        "feb":"02",
        "mar":"03",
        "apr":"04",
        "may":"05",
        "jun":"06",
        "jul":"07",
        "aug":"08",
        "sep":"09",
        "oct":"10",
        "nov":"11",
        "dec":"12"
}
def getLastRevisionDate(repoPath):
    """ @rtype: dateString
    @return: The date of the last revision of the given repository
    """
    # determine if this is a git or hg repo
    if os.path.exists((u"%s.git/"%(repoPath)).encode(getFsEncoding(logLevel=False))):
        cmd_args = ["git","log","-n","1"]
    elif os.path.exists(repoPath+".hg/"):
        cmd_args = ["hg","tip"]
    else:
        raise Exception("%s is not a git nor a hg repo"%repoPath)
    pipe = subprocess.Popen(cmd_args,stdout=subprocess.PIPE)
    out,err = pipe.communicate()
    for l in out.split('\n'):
        if l.lower().startswith("date"):
            lspl = l.split()
            datestr = lspl[3]
            datestr += "/"
            datestr += m2m[lspl[2].lower()]
            datestr += "/"
            datestr += lspl[5]
            return datestr
    return "01/01/1970"

class DirNFileDialog(QFileDialog):
    """ File dialog which can open a directory *OR* a file
    """
    def accept(self):
        files = self.selectedFiles()
        if (files.isEmpty()):
            return
        self.emit(SIGNAL("filesSelected(QStringList)"),files)
        super(QFileDialog,self).accept()

def isUnixText(filename):
    """ Determine if a file has Linux or Windows line separators
    """
    f = open(filename.encode(getFsEncoding(logLevel=False)),'r')
    srt = f.read()
    f.close()
    return not ('\r\n' in srt)

def dos2unix(filename):
    """ Traduce a windows-style text file into a Linux-style text file
    """
    f=open(filename.encode(getFsEncoding(logLevel=False)),'r')
    sw = f.read().replace('\r\n', '\n')
    f.close()
    f=open(filename.encode(getFsEncoding(logLevel=False)),'w')
    f.write(sw)
    f.close()

def readlinesWindows(filename):
    f=open(filename.encode(getFsEncoding(logLevel=False)),'rU')
    li=f.readline()
    res=[]
    while li!='':
        res.append(li)
        li=f.readline()
    return res

def getFsEncoding(logLevel=False):
    fSCoding = sys.getfilesystemencoding()
    if fSCoding == None :
        locale.setlocale(locale.LC_ALL,)
        fSCoding = sys.getfilesystemencoding()
        if fSCoding == None :
            if locale.getpreferredencoding() != None :
                fSCoding = locale.getpreferredencoding()
                log(logLevel,"File system encoding set to system preferred encoding : %s" % fSCoding)
            else :
                fSCoding = "iso-8859-1" # "u"UTF-8"
                log(logLevel,"Unable to determine file system encoding. Set to default encoding : %s" % fSCoding)
        else :
            log(logLevel,"File system encoding set from LC_ALL : %s" % fSCoding)
    else :
        log(logLevel,"File system encoding : %s" % fSCoding)
    return fSCoding




