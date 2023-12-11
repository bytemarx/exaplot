import orbital


try:
    orbital.plot()
except TypeError as e:
    assert(str(e) == "plot() missing 1 required positional argument: 'data_set'")
