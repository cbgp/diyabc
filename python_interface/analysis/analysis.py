# -*- coding: utf-8 -*-

class Analysis():
    def __init__(self,name,category):
        self.name = name
        self.category = category
        self.chosenSc = 0
        self.candidateScList = []
        self.computationParameters = ""
        self.mutationModel = None
        self.histParams = None
        self.drawn = True
        self.params = ""

    def getChosenSc(self):
        return self.chosenSc

    def getCandidateScList(self):
        return self.candidateScList

    def getComputationParameters(self):
        return self.computationParameters

