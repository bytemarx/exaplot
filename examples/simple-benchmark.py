import math
import time

from exaplot import RunParam, init, plot, stop, datafile

init(3, duration=RunParam(float, display="Duration (s)"))

simple_plot = plot[1]
simple_plot.title = "Simple Plot Example"
simple_plot.two_dimen.autorescale_axes = True

benchmark_plot = plot[2]
benchmark_plot.title = "Benchmark"
benchmark_plot.two_dimen.autorescale_axes = True

benchmark_baseline_plot = plot[3]
benchmark_baseline_plot.title = "Benchmark Baseline"
benchmark_baseline_plot.two_dimen.autorescale_axes = True


def foo(x, y):
    return x + y


def run(duration: float):
    x_i = -2.5
    x_f = 2.5
    x_delta = 0.001
    t_delta = duration / ((x_f - x_i) / x_delta)

    def f(x):
        return math.sin(20 * math.pi * x) * math.exp(-(x**2))

    x = x_i
    while x <= x_f and not stop():
        t_i = time.time()
        simple_plot(x, f(x))
        dt = time.time() - t_i
        benchmark_plot(x, dt)
        t_i = time.time()
        foo(x, dt)
        dt = time.time() - t_i
        benchmark_baseline_plot(x, dt)
        time.sleep(t_delta)
        x += x_delta
