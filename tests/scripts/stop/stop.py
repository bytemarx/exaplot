import orbital


assert(not stop())
assert(not orbital.stop(__name__))
assert(not orbital.stop(int(__name__[11:])))
orbital.plot(0)
assert(stop())
assert(orbital.stop(__name__))
assert(orbital.stop(int(__name__[11:])))
