from exaplot import datafile


try:
    datafile(None)
except TypeError as e:
    assert(str(e) == "datafile() takes 0 positional arguments but 1 was given")

try:
    datafile(x=None)
except TypeError as e:
    assert(str(e) == "'x' is an invalid keyword argument for datafile()")

try:
    datafile(path="/")
except TypeError as e:
    assert(str(e) == "'path' argument must be type 'PathLike' or 'Callable'")
