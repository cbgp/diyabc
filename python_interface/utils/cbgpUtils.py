# -*- coding: utf-8 -*-

from threading import Thread
import cmd

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
import output
import os.path

class Documentator():
    """ classe qui extrait la documentation interne en lisant un fichier xml 
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

def readRefTableSize(reftablefile):
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
    if os.path.exists(reftablefile):
        binint = array.array('i')
        f = open(reftablefile,'rb')
        binint.read(f,numSc+2)
        f.close()
        nbRec = binint[-1]
        return nbRec
    else:
        return None
