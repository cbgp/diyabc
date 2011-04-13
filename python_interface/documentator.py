# -*- coding: utf-8 -*-

class Documentator():
    """ classe qui extrait la documentation à partir de mots clés
    """
    def __init__(self,parent=None):
        self.parent=parent
        self.dicoDoc = {}

    def getDocString(self,key):
        """ retourne la doc liée à key
        """
        result = None
        if key in self.dicoDoc.keys():
            result =  self.dicoDoc[key]
        return result


