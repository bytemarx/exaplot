from numbers import Real
from os import PathLike
from pathlib import Path
from typing import Callable, Generic, Sequence, TypeVar, overload

RunParamType = TypeVar('RunParamType', str, int, float)
class RunParam(Generic[RunParamType]):
    @overload
    def __init__(self, default: RunParamType, /, display: str = None) -> None:
        """Run parameter with a default value and an optional display
        representation.

        :param default: default value
        :type default: RunParamType
        :param display: display string, defaults to None
        :type display: str, optional
        """
    @overload
    def __init__(self, type: type[RunParamType], /, display: str = None) -> None:
        """Run parameter with no default value and an optional display
        representation.

        :param type: value's type
        :type type: type[RunParamType]
        :param display: display string, defaults to None
        :type display: str, optional
        """
@overload
def init(plots: int = 1, /, **params: str | int | float | RunParam[str] | RunParam[int] | RunParam[float]) -> None:
    """Set the number of plots and the parameters for the `run` function.

    :param plots: number of plots, defaults to 1
    :type plots: int, optional
    """
@overload
def init(plots: list[tuple[int, int, int, int]], /, **params: str | int | float | RunParam[str] | RunParam[int] | RunParam[float]) -> None:
    """Set the plot arrangement and the parameters for the `run` function.

    :param plots: plot arrangement
    :type plots: list[tuple[int, int, int, int]]
    """
def datafile(
        *,
        enable: bool = True,
        path: PathLike | Callable[[], PathLike] = Path("data.hdf5"),
        prompt: bool = False,
    ) -> None:
    """Configure data file settings.

    :param enable: enable or disable writing to a data file, defaults to True
    :type enable: bool, optional
    :param path: data file path, defaults to Path("data.hdf5")
    :type path: PathLike | Callable[[], PathLike], optional
    :param prompt: prompt before running, defaults to False
    :type prompt: bool, optional
    """
def stop() -> bool:
    """Check if a stop signal has been received.

    :rtype: bool
    """
def breakpoint() -> None:
    """Alternative method of stopping. This function will raise a
    special `BaseException`-derived exception if a stop signal has been
    received. If raised during a run, the application will not prompt
    the user with an error (same behavior as if the run completed).
    
    Most scripts should be fine using this, but some scripts may warrant
    extra caution when using code/libraries that don't clean up properly
    in the event of an exception.
    """
def msg(message: str, /, append: bool = False) -> None:
    """Set the message in the script message box.

    :param message: message string
    :type message: str
    :param append: append to the current message, defaults to False
    :type append: bool, optional
    """
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
        @property
        def autorescale_axes(self) -> bool: ...
        @autorescale_axes.setter
        def autorescale_axes(self, value: bool) -> None:...
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
        @property
        def autorescale_axes(self) -> bool: ...
        @autorescale_axes.setter
        def autorescale_axes(self, value: bool) -> None:...
        @property
        def autorescale_data(self) -> bool: ...
        @autorescale_data.setter
        def autorescale_data(self, value: bool) -> None:...
class _Plot:
    @overload
    def __call__(self) -> None:
        """Clears the plot."""
    @overload
    def __call__(self, x: Real, y: Real, *, write: bool = True) -> None:
        """Plots a data point to the 2D plot.

        :param x: x-value
        :type x: Real
        :param y: y-value
        :type y: Real
        :param write: write data to disk, defaults to True
        :type write: bool, optional
        """
    @overload
    def __call__(self, x: Sequence[Real], y: Sequence[Real], *, write: bool = True) -> None:
        """Plots multiple data points to the 2D plot.

        :param x: x-values
        :type x: Sequence[Real]
        :param y: y-values
        :type y: Sequence[Real]
        :param write: write data to disk, defaults to True
        :type write: bool, optional
        """
    @overload
    def __call__(self, col: int, row: int, value: Real, *, write: bool = True) -> None:
        """Plots a value to a specifed cell in the color map.

        :param col: target column (from left to right)
        :type col: int
        :param row: target row (from down to up)
        :type row: int
        :param value: cell value
        :type value: Real
        :param write: write data to disk, defaults to True
        :type write: bool, optional
        """
    @overload
    def __call__(self, row: int, values: Sequence[Real], *, write: bool = True) -> None:
        """Plots a row of cell values to the color map.

        :param row: target row (from down to up)
        :type row: int
        :param values: row values
        :type values: Sequence[Real]
        :param write: write data to disk, defaults to True
        :type write: bool, optional
        """
    @overload
    def __call__(self, frame: Sequence[Sequence[Real]], *, write: bool = True) -> None:
        """Plots a frame of cell values to the color map.

        :param frame: sequence of rows
        :type frame: Sequence[Sequence[Real]]
        :param write: write data to disk, defaults to True
        :type write: bool, optional
        """
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
