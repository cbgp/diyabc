# -*- coding: utf-8 -*-

class Documentator():
    """ classe qui extrait la documentation à partir de mots clés
    """
    def __init__(self,parent=None):
        self.parent=parent
        self.dicoDoc = {
                "nbScLabel":"Number of scenario set in the historical model",
                "nbParamLabel":"Number of parameters set in the historical model",
                }

    def getDocString(self,key):
        """ retourne la doc liée à key
        """
        result = None
        if key in self.dicoDoc.keys():
            result =  self.dicoDoc[key]
        return result


