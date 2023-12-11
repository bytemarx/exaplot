import orbital


try:
    orbital.msg(None)
except TypeError as e:
    assert(str(e) == "msg() argument 1 must be str, not None")

try:
    orbital.msg(1)
except TypeError as e:
    assert(str(e) == "msg() argument 1 must be str, not int")

try:
    orbital.msg("", foo=None)
except TypeError as e:
    assert(str(e) == "'foo' is an invalid keyword argument for msg()")
