def ite(rang):
    i = 0
    while i<rang:
        yield i
        i+=1
    raise Exception("fini")

ii = ite(3)
try:
    while True:
        print ii.next()
except Exception,e:
    print "on a fini"
print "ow yeah"
