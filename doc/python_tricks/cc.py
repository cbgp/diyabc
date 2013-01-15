import time

class plop(object):
    def __init__(self):
        pass
    def __call__(self,p):
        print p
        return p

a = plop()
b = a(33)
print b
