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
        self.mutationModelFixed = ""
        self.mutationModelDrawn = ""
        self.sumStatsTHDico = {}
        self.sumStatsConfDico = {}
        self.histParamsFixed = None
        self.histParamsDrawn = None
        self.condTxtList = []
        self.drawn = None
        self.params = ""
        self.aParams = None
        self.fda = None
        self.logreg = None

        self.status = status

    def getChosenSc(self):
        return self.chosenSc

    def getCandidateScList(self):
        return self.candidateScList

    def getComputationParameters(self):
        return self.computationParameters

