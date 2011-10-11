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
            output.notify(self.parent,"documentation not found","The xml documentation file %s was not found"%xmlFile)

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
        return result


