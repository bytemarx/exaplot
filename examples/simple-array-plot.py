import math
import time

from zetaplot import RunParam, breakpoint, init, msg, plot


init(ring_density=RunParam(5.0, "Ring Density"))

n_rows = 400
n_cols = 1000
x_min = -5.0
x_max =  5.0
y_min = -10.0
y_max =  10.0

plot = plot[1]
plot.two_dimen.x_range = x_min, x_max
plot.two_dimen.y_range = -1, 1
x_scale = (x_max - x_min) / n_cols
y_scale = (y_max - y_min) / n_rows
dt = 0.05


def _run(ring_density: float):
    for T in range(2):
        dT = T * dt
        for row in range(n_rows):
            x_arr = [None] * n_cols
            y_arr = [None] * n_cols
            y = y_scale * row + y_min
            for col in range(n_cols):
                breakpoint()
                x = x_scale * col + x_min
                r = ring_density * math.sqrt(x**2 + y**2) + 0.01
                z = 4 * x * (math.cos((r + 2))/(r - math.sin((r + 2) / r)))
                x_arr[col] = x
                y_arr[col] = z
            plot()
            plot(x_arr, y_arr)
            msg(f"Time: {round(n_rows * dT + row * dt, 1)}", append=False)
            time.sleep(dt)


def run(ring_density: float):
    plot.two_dimen.line.type = "impulse"
    plot.two_dimen.line.color = "#A68435"
    plot.two_dimen.points.shape = "none"
    _run(ring_density)

    plot.two_dimen.line.type = "none"
    plot.two_dimen.line.color = "#BF4040"
    plot.two_dimen.points.shape = "peace"
    plot.two_dimen.points.size = 12.
    _run(ring_density)
