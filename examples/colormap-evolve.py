import math
import orbital
import time


orbital.init("Ring Density")
plot = orbital.plot[1]
plot.title = "Color Map Example"
plot.color_map.data_size = 100, 100
plot.color_map.x_range = -5.0, 5.0
plot.color_map.y_range = -5.0, 5.0
plot.color_map.z_range = -1.0, 1.0
plot.color_map.show()


def run(**kwargs):
    ring_density = float(kwargs["Ring Density"])

    def f(x, y, phi):
        r = math.sqrt(x**2 + y**2)
        if r > 14.25:
            return 0
        r = ring_density * r + 0.01
        return 4 * x * (math.cos(r + 2 + phi) / (r - math.sin(r + 2 + phi) / r))
    
    def plot_frame(phi):
        for row in range(100):
            y = 0.1 * row - 5.0
            values = [f(0.1 * col - 5.0, y, phi) for col in range(100)]
            plot(row, values)
    
    for t in range(200):
        plot_frame(-0.1 * t)
        time.sleep(0.05)
