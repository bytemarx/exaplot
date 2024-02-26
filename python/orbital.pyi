from numbers import Real
from typing import overload

@overload
def init(*params: str, plots: int = 1) -> None: ...
@overload
def init(*params: str, plots: list[tuple[int, int, int, int]]) -> None: ...
def msg(message: str, /, append: bool = False) -> None: ...
class _PlotProperties:
    class MinSize:
        @property
        def w(self) -> int: ...
        @w.setter
        def w(self, value: int) -> None: ...
        @property
        def h(self) -> int: ...
        @h.setter
        def h(self, value: int) -> None: ...
    class Range:
        @property
        def min(self) -> Real: ...
        @min.setter
        def min(self, value: Real) -> None: ...
        @property
        def max(self) -> Real: ...
        @max.setter
        def max(self, value: Real) -> None: ...
    class Line:
        @property
        def type(self) -> str: ...
        @type.setter
        def type(self, value: str) -> None: ...
        @property
        def color(self) -> str: ...
        @color.setter
        def color(self, value: str) -> None: ...
        @property
        def style(self) -> str: ...
        @style.setter
        def style(self, value: str) -> None: ...
    class Points:
        @property
        def shape(self) -> str: ...
        @shape.setter
        def shape(self, value: str) -> None: ...
        @property
        def color(self) -> str: ...
        @color.setter
        def color(self, value: str) -> None: ...
        @property
        def size(self) -> float: ...
        @size.setter
        def size(self, value: float) -> None: ...
    class DataSize:
        @property
        def x(self) -> int: ...
        @x.setter
        def x(self, value: int) -> None: ...
        @property
        def y(self) -> int: ...
        @y.setter
        def y(self, value: int) -> None: ...
    class Color:
        @property
        def min(self) -> str: ...
        @min.setter
        def min(self, value: str) -> None: ...
        @property
        def max(self) -> str: ...
        @max.setter
        def max(self, value: str) -> None: ...
    class _Tab:
        def show(self) -> None: ...
        @property
        def x_range(self) -> _PlotProperties.Range: ...
        @x_range.setter
        def x_range(self, value: tuple[int, int]) -> None: ...
        @property
        def y_range(self) -> _PlotProperties.Range: ...
        @y_range.setter
        def y_range(self, value: tuple[int, int]) -> None: ...
    class TwoDimen(_Tab):
        @property
        def line(self) -> _PlotProperties.Line: ...
        @line.setter
        def line(self, value: tuple[str, str, str]) -> None: ...
        @property
        def points(self) -> _PlotProperties.Points: ...
        @points.setter
        def points(self, value: tuple[str, str, float]) -> None: ...
    class ColorMap(_Tab):
        @property
        def z_range(self) -> _PlotProperties.Range: ...
        @z_range.setter
        def z_range(self, value: tuple[int, int]) -> None: ...
        @property
        def data_size(self) -> _PlotProperties.DataSize: ...
        @data_size.setter
        def data_size(self, value: tuple[int, int]) -> None: ...
        @property
        def color(self) -> _PlotProperties.Color: ...
        @color.setter
        def color(self, value: tuple[str, str]) -> None: ...
class _Plot:
    @overload
    def __call__(self, x: Real, y: Real) -> None: ...
    @overload
    def __call__(self, x: list[Real], y: list[Real]) -> None: ...
    @overload
    def __call__(self, col: int, row: int, value: Real) -> None: ...
    @overload
    def __call__(self, row: int, values: list[Real]) -> None: ...
    @overload
    def __call__(self, frame: list[list[Real]]) -> None: ...
    @property
    def title(self) -> str: ...
    @title.setter
    def title(self, value: str) -> None: ...
    @property
    def x_axis(self) -> str: ...
    @x_axis.setter
    def x_axis(self, value: str) -> None: ...
    @property
    def y_axis(self) -> str: ...
    @y_axis.setter
    def y_axis(self, value: str) -> None: ...
    @property
    def min_size(self) -> _PlotProperties.MinSize: ...
    @min_size.setter
    def min_size(self, value: tuple[int, int]) -> None: ...
    @property
    def two_dimen(self) -> _PlotProperties.TwoDimen: ...
    @property
    def color_map(self) -> _PlotProperties.ColorMap: ...
plot: list[_Plot]