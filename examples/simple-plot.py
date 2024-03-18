import math
import time

from zetaplot import RunParam, init, plot, stop

init(duration=RunParam(float, display="Duration (s)"))
plot = plot[1]
plot.title = "Simple Plot Example"
plot.two_dimen.x_range = -2.5, 2.5
plot.two_dimen.y_range = -1, 1
plot.two_dimen.autorescale_axes = True


def run(duration: float):
    x_i = -2.5
    x_f = 2.5
    x_delta = 0.001
    t_delta = duration / ((x_f - x_i) / x_delta)

    def f(x):
        return math.sin(20 * math.pi * x) * math.exp(-(x**2))

    x = x_i
    while x <= x_f and not stop():
        plot(x, f(x))
        time.sleep(t_delta)
        x += x_delta
