import math
import time

from orbital import RunParam, init, plot


init(duration = RunParam(float, display="Duration (s)"))
plot = plot[1]
plot.title = "Simple Plot Example"


def run(duration: float):
    x_i = -2.5
    x_f = 2.5
    x_delta = 0.001
    t_delta = duration / ((x_f - x_i) / x_delta)

    def f(x):
        return math.sin(20 * math.pi * x) * math.exp(-x**2)

    x = x_i
    while x <= x_f:
        plot(x, f(x))
        time.sleep(t_delta)
        x += x_delta
