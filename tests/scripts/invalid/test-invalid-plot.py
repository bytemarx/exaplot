import orbital


try:
    orbital._plot()
except TypeError as e:
    assert(str(e) == "plot() missing 1 required positional argument: 'data_set'")

try:
    orbital._plot("")
except TypeError as e:
    assert(str(e) == "plot() data_set parameter must be type 'int'")

try:
    orbital._plot(1, foo=None)
except TypeError as e:
    assert(str(e) == "_orbital.plot() takes no keyword arguments")

try:
    orbital._plot(1, "")
except TypeError as e:
    assert(str(e) == "must be real number, not str")

try:
    orbital.plot[1]("")
except TypeError as e:
    assert(str(e) == "must be real number, not str")

try:
    orbital._plot(1, 1, "")
except TypeError as e:
    assert(str(e) == "must be real number, not str")

try:
    orbital.plot[1](1, "")
except TypeError as e:
    assert(str(e) == "must be real number, not str")

try:
    orbital._plot(1, [None])
except TypeError as e:
    assert(str(e) == "must be real number, not NoneType")

try:
    orbital.plot[1]([None])
except TypeError as e:
    assert(str(e) == "must be real number, not NoneType")
