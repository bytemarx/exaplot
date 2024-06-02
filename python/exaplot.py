from numbers import Real
from os import PathLike
from typing import Callable

from _exaplot import ( # type: ignore
    RunParam,
    init,
    stop,
    msg,
    datafile as _datafile,
    plot as _plot,
    _Interrupt,
    _set_plot_property,
    _get_plot_property,
    _show_plot,
)


def breakpoint():
    if stop():
        raise _Interrupt


def datafile(**kwargs):
    if (
        (path := kwargs.get("path"))
        and not (isinstance(path, PathLike) or isinstance(path, Callable))
    ):
        raise TypeError("'path' argument must be type 'PathLike' or 'Callable'")
    return _datafile(**kwargs)


class PlotProperties:
    class _Property:
        def __init_subclass__(cls, /, **kwargs):
            for p, t in kwargs.items():
                def getter(self: 'PlotProperties._Property', p=p):
                    return _get_plot_property(self._n, f"{self._id}.{p}")
                def setter(self: 'PlotProperties._Property', value, p=p, t=t):
                    if not isinstance(value, t):
                        raise TypeError(f"{self._id}.{p} must be type '{t.__name__}'")
                    _set_plot_property(self._n, f"{self._id}.{p}", value)
                prop = property(fget=getter, fset=setter)
                setattr(cls, p, prop)

        def __init__(self, n: int, id: str):
            self._n = n
            self._id = id

    class MinSize(_Property, w=int, h=int):
        def __repr__(self):
            return f"{self.w, self.h}"

    class Range(_Property, min=Real, max=Real):
        def __repr__(self):
            return f"{self.min, self.max}"

    class Line(_Property, type=str, color=str, style=str):
        def __repr__(self):
            return f"{self.type, self.color, self.style}"

    class Points(_Property, shape=str, color=str, size=float):
        def __repr__(self):
            return f"{self.shape, self.color, self.size}"

    class DataSize(_Property, x=int, y=int):
        def __repr__(self):
            return f"{self.x, self.y}"

    class Color(_Property, min=str, max=str):
        def __repr__(self):
            return f"{self.min, self.max}"

    class _Tab(_Property):
        def show(self):
            plot_type = {
                "two_dimen": 0,
                "color_map": 1,
            }[self._id]
            _show_plot(self._n, plot_type)

        @property
        def x_range(self):
            return PlotProperties.Range(self._n, f"{self._id}.x_range")

        @x_range.setter
        def x_range(self, value: tuple[int, int]):
            if not isinstance(value, tuple) or len(value) != 2:
                raise TypeError(f"{self._id}.x_range value must be type 'tuple[int, int]'")
            self.x_range.min = value[0]
            self.x_range.max = value[1]

        @property
        def y_range(self):
            return PlotProperties.Range(self._n, f"{self._id}.y_range")

        @y_range.setter
        def y_range(self, value: tuple[int, int]):
            if not isinstance(value, tuple) or len(value) != 2:
                raise TypeError(f"{self._id}.y_range value must be type 'tuple[int, int]'")
            self.y_range.min = value[0]
            self.y_range.max = value[1]

    class TwoDimen(_Tab, autorescale_axes=bool):
        @property
        def line(self):
            return PlotProperties.Line(self._n, f"{self._id}.line")

        @line.setter
        def line(self, value: tuple[str, str, str]):
            if not isinstance(value, tuple) or len(value) != 3:
                raise TypeError(f"{self._id}.line value must be type 'tuple[str, str, str]'")
            self.line.type = value[0]
            self.line.color = value[1]
            self.line.style = value[2]

        @property
        def points(self):
            return PlotProperties.Points(self._n, f"{self._id}.points")

        @points.setter
        def points(self, value: tuple[str, str, float]):
            if not isinstance(value, tuple) or len(value) != 3:
                raise TypeError(f"{self._id}.points value must be type 'tuple[str, str, float]'")
            self.points.shape = value[0]
            self.points.color = value[1]
            self.points.size = value[2]

    class ColorMap(_Tab, autorescale_axes=bool, autorescale_data=bool):
        @property
        def z_range(self):
            return PlotProperties.Range(self._n, f"{self._id}.z_range")

        @z_range.setter
        def z_range(self, value: tuple[int, int]):
            if not isinstance(value, tuple) or len(value) != 2:
                raise TypeError(f"{self._id}.z_range value must be type 'tuple[int, int]'")
            self.z_range.min = value[0]
            self.z_range.max = value[1]

        @property
        def data_size(self):
            return PlotProperties.DataSize(self._n, f"{self._id}.data_size")

        @data_size.setter
        def data_size(self, value: tuple[int, int]):
            if not isinstance(value, tuple) or len(value) != 2:
                raise TypeError(f"{self._id}.data_size value must be type 'tuple[int, int]'")
            self.data_size.x = value[0]
            self.data_size.y = value[1]

        @property
        def color(self):
            return PlotProperties.Color(self._n, f"{self._id}.color")

        @color.setter
        def color(self, value: tuple[str, str]):
            if not isinstance(value, tuple) or len(value) != 2:
                raise TypeError(f"{self._id}.color value must be type 'tuple[str, str]'")
            self.color.min = value[0]
            self.color.max = value[1]


class Plot:
    def __init__(self, n: int):
        self._n = n

    def __call__(self, *args, **kwargs):
        return _plot(self._n, *args, **kwargs)

    @property
    def title(self):
        return _get_plot_property(self._n, "title")

    @title.setter
    def title(self, value: str):
        if not isinstance(value, str):
            raise TypeError("title must be type 'str'")
        _set_plot_property(self._n, "title", value)

    @property
    def x_axis(self):
        return _get_plot_property(self._n, "x_axis")

    @x_axis.setter
    def x_axis(self, value: str):
        if not isinstance(value, str):
            raise TypeError("x_axis must be type 'str'")
        _set_plot_property(self._n, "x_axis", value)

    @property
    def y_axis(self):
        return _get_plot_property(self._n, "y_axis")

    @y_axis.setter
    def y_axis(self, value: str):
        if not isinstance(value, str):
            raise TypeError("y_axis must be type 'str'")
        _set_plot_property(self._n, "y_axis", value)

    @property
    def min_size(self):
        return PlotProperties.MinSize(self._n, "min_size")

    @min_size.setter
    def min_size(self, value: tuple[int, int]):
        if not isinstance(value, tuple) or len(value) != 2:
            raise TypeError("min_size value must be type 'tuple[int, int]'")
        self.min_size.w = value[0]
        self.min_size.h = value[1]

    @property
    def two_dimen(self):
        return PlotProperties.TwoDimen(self._n, "two_dimen")

    @property
    def color_map(self):
        return PlotProperties.ColorMap(self._n, "color_map")


class _Plots:
    def __init__(self):
        self._plots: dict[int, Plot] = {1: Plot(1)}

    def __getitem__(self, index: int):
        try:
            return self._plots[index]
        except KeyError:
            self._plots[index] = Plot(index)
            return self._plots[index]


plot = _Plots()
