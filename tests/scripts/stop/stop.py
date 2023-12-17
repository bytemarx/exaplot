import orbital


assert(not stop())
assert(not orbital.stop(__name__))
orbital.plot(0)
assert(stop())
assert(orbital.stop(__name__))
