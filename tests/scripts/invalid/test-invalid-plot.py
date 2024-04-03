from exaplot import _plot, plot


try:
    _plot()
except TypeError as e:
    assert(str(e) == "plot() missing 1 required positional argument: 'plot_id'")

try:
    _plot("")
except TypeError as e:
    assert(str(e) == "plot() 'plot_id' argument must be type 'int'")

try:
    _plot(1, foo=None)
except TypeError as e:
    assert(str(e) == "_exaplot.plot() takes no keyword arguments")

try:
    _plot(1, "")
except TypeError as e:
    assert(str(e) == "plot() takes 2 positional arguments but 1 were given")

try:
    plot[1]("")
except TypeError as e:
    assert(str(e) == "plot() takes 2 positional arguments but 1 were given")

try:
    plot[1](0, 0, 0)
except TypeError as e:
    assert(str(e) == "plot() takes 2 positional arguments but 3 were given")

try:
    _plot(1, 1, "")
except TypeError as e:
    assert(str(e) == "must be real number, not str")

try:
    plot[1](1, "")
except TypeError as e:
    assert(str(e) == "must be real number, not str")

try:
    _plot(1, [None])
except TypeError as e:
    assert(str(e) == "plot() takes 2 positional arguments but 1 were given")

try:
    plot[1]([0], [0], [0])
except TypeError as e:
    assert(str(e) == "plot() takes 2 positional arguments but 3 were given")

try:
    plot[1]([0], 0)
except TypeError as e:
    assert(str(e) == "plot() 'y' argument must be type 'Sequence'")

try:
    plot[1]([0], [""])
except TypeError as e:
    assert(str(e) == "must be real number, not str")
