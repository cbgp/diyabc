# -*- coding: utf-8 -*-

class Analysis():
    """ classe pour représenter une analyse, contient toutes les informations nécessaires
    au lancement du calcul lié à l'analyse
    """
    def __init__(self,name,category,status="new"):
        self.name = name
        self.category = category
        self.chosenSc = None
        self.candidateScList = []
        self.computationParameters = ""
        self.mutationModel = ""
        self.sumStatsTHDico = {}
        self.sumStatsConfDico = {}
        self.histParams = None
        self.condTxtList = []
        self.drawn = None
        self.params = ""
        self.aParams = None
        self.fda = None

        self.status = status

    def getChosenSc(self):
        return self.chosenSc

    def getCandidateScList(self):
        return self.candidateScList

    def getComputationParameters(self):
        return self.computationParameters

