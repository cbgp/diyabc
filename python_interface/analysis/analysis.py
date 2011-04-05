# -*- coding: utf-8 -*-

class Analysis():
    def __init__(self,name,category):
        self.name = name
        self.category = category
        self.chosenSc = 0
        self.candidateScList = []
        self.computationParameters = ""
        self.mutationModel = None
        self.sumStatsDico = {}
        self.histParams = None
        self.condTxtList = []
        self.drawn = True
        self.params = ""
        self.aParams = None

    def getChosenSc(self):
        return self.chosenSc

    def getCandidateScList(self):
        return self.candidateScList

    def getComputationParameters(self):
        return self.computationParameters

