def mydeco(fun):
    def plop(*args, **kwargs):
        print "je passe dans plop"
        if fun(2,True) != 4 or fun(4,True) != 8:
            raise Exception("pas content")
        return fun(*args, **kwargs)
    return plop

@mydeco
def myfun(a,silent=False):
    if not silent:
        print "myfun : %s"%a
    return a*3

myfun(4)
