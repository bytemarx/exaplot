import zetaplot


try:
    zetaplot.msg(None)
except TypeError as e:
    assert(str(e) == "msg() argument 1 must be str, not None")

try:
    zetaplot.msg(1)
except TypeError as e:
    assert(str(e) == "msg() argument 1 must be str, not int")

try:
    zetaplot.msg("", foo=None)
except TypeError as e:
    assert(str(e) == "'foo' is an invalid keyword argument for msg()")

try:
    zetaplot.msg("", False, None)
except TypeError as e:
    assert(str(e) == "msg() takes at most 2 arguments (3 given)")
