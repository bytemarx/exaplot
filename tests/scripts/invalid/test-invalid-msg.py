from exaplot import msg


try:
    msg(None)
except TypeError as e:
    assert(str(e) == "msg() argument 1 must be str, not None")

try:
    msg(1)
except TypeError as e:
    assert(str(e) == "msg() argument 1 must be str, not int")

try:
    msg("", foo=None)
except TypeError as e:
    assert(str(e) == "'foo' is an invalid keyword argument for msg()")

try:
    msg("", False, None)
except TypeError as e:
    assert(str(e) == "msg() takes at most 2 arguments (3 given)")
