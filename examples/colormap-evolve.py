"""Based on QCustomPlot Color Map Demo:
https://www.qcustomplot.com/index.php/demos/colormapdemo
"""

import math
import time

from exaplot import RunParam, init, plot, breakpoint

init(ring_density=RunParam(5.0, "Ring Density"))
plot = plot[1]
plot.title = "Color Map Example"
plot.color_map.data_size = 100, 100
plot.color_map.x_range = -5.0, 5.0
plot.color_map.y_range = -5.0, 5.0
plot.color_map.z_range = -1.0, 1.0
plot.color_map.show()


def run(ring_density: float):
    def f(x, y, phi):
        r = ring_density * math.sqrt(x**2 + y**2) + 0.01
        return 4 * x * (math.cos(r + 2 + phi) / (r - math.sin(r + 2 + phi) / r))

    for t in range(500):
        breakpoint()
        plot([
            [f(0.1 * col - 5.0, 0.1 * row - 5.0, -0.1 * t) for col in range(100)]
            for row in range(100)
        ])
        time.sleep(0.02)
