class curried(object):
  """
  Decorator that returns a function that keeps returning functions
  until all arguments are supplied; then the original function is
  evaluated.
  """

  def __init__(self, func, *a):
    self.func = func
    self.args = a
  def __call__(self, *a):
    args = self.args + a
    if len(args) < self.func.func_code.co_argcount:
      return curried(self.func, *args)
    else:
      return self.func(*args)


@curried
def add(a, b):
    return a + b

add1 = add(1)

print add1(2)
aa=  add(5)
print aa(5)
