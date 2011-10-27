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
    """ prompt qui execute le code python passé par l'utilisateur
    à l'aide de la commande do
    """
    def do_do(self,line):
        try:
            exec line
        except Exception,e:
            print e
    
    def do_EOF(self, line):
        return True

class cmdThread(Thread):
    """ Thread de lancement du prompt
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
    """ Classe qui extrait la documentation interne en lisant un fichier xml 
    généré à partir des sources latex de la notice de DIYABC. Cette classe permet aussi
    la consultation de cette documentation à partir de mots clés (l'objectName en l'occurence)
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
        """ charge le fichier xml pour remplir le dico de documentation
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
        """ retourne la doc liée à key
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
    """ fonction qui nettoie le dossier logFolder de 
    tous les fichiers plus vieux que nbDaysOld
    SSI le dossier de logs est plus lourd que sizeThreshold Mo
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
    except Exception,e:
        raise Exception("Log rotation failed\n%s"%e)

import array

def readRefTableSize(reftablefile):
    """ lit le nombre de records dans l'entete de 
    la reftable binaire
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
    """ lit le nombre de records concernant un scenario
    donnée dans l'entête de la reftable binaire
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
    """ Classe qui remplace stdout et stderr pour logger sur 
    la sortie standard (ancienne stdout), dans un fichier et qui appelle
    une fonction externe simultanément
    ATTENTION effet de bord : stderr n'est pas différentié de stdout dans le
    fichier de log et dans la fonction externe
    La fonction externe doit s'appeller comme cela :
    showExternal("string to display")
    """
    def __init__(self, name, out_or_err, showExternal=None):
        """ remplace stdout ou stderr
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
    """ Affiche le message fancyfied
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
