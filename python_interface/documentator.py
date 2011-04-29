# -*- coding: utf-8 -*-

from xml.dom.minidom import parse

class Documentator():
    """ classe qui extrait la documentation à partir de mots clés
    """
    def __init__(self,xmlFile,parent=None):
        self.parent=parent
        self.xmlFile = xmlFile
        self.dicoDoc = {
                "nbScLabel":"Number of scenario set in the historical model",
                "nbParamLabel":"Number of parameters set in the historical model",
                }
        self.loadDocFile()

    def loadDocFile(self):
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


