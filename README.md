# ExaPlot

A framework for plotting and collecting real-time data with Python.

![ExaPlot Preview](https://raw.githubusercontent.com/bytemarx/exaplot-gifs/5b231381077c7d591c79a28ce79b33d0f093a5ca/exaplot.gif)


A trivial example script:
```python
import math

from exaplot import datafile, init, plot, stop

# Initialize the data file and plots (we'll just use the default
# settings for each), and set parameters for the 'run' function.
datafile()
init(frequency=5.0)


def run(frequency: float):
    x = -10.0
    while x <= 10.0 and not stop():
        y = 10 * math.sin(frequency * x) * math.exp(-x**2 / 10.0)
        plot[1](x, y)
        x += 0.001
```


### Supported Platforms
- Linux
- Windows


## Building

Requires:
- CMake 3.22+
- [CPython dependencies](https://devguide.python.org/getting-started/setup-building/index.html#build-dependencies)
- Qt6
- HDF5 dependencies

### Linux (Bash)
```sh
git submodule update --init --recursive
cmake -DCMAKE_BUILD_TYPE=Release -B build/
make -j -C build/
```

### Windows (PowerShell)
Set
- `QT_DIR` to your Qt6 MSVC folder
- `MSBUILD` to your `MSBuild.exe`
- `VCPKG_ROOT` to your vcpkg folder (with [zlib](https://vcpkg.io/en/package/zlib.html) installed)

and run the `winbuild.ps1` PowerShell script:
```powershell
git submodule update --init --recursive
$Env:QT_DIR = "C:\Qt\6.4.0\msvc2019_64"
$Env:MSBUILD = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
$Env:VCPKG_ROOT = "C:\vcpkg"
.\winbuild.ps1
```

Built applications can be found in `build/bin/<BUILD-TYPE>/` where `<BUILD-TYPE>` corresponds to
the build type specified by `CMAKE_BUILD_TYPE` (typically either `Release` or `Debug`).


## Developing

Build Variables:
- `-DCMAKE_BUILD_TYPE=Debug`
    - Debug build
- `-DCMAKE_BUILD_TYPE=Release`
    - Release build
- `-DWITH_TESTS=1`
    - Build with tests (only necessary for non-debug builds)
- `-DAPP_PLUGINS=1`
    - Build with Qt plugin libraries


## Application Configuration
The `EXACONFIG` environment variable can be used to specify a TOML-based configuration file:
```sh
EXACONFIG='docs/sample-config.toml' build/bin/Release/exaplot
```

### Adding External Libraries
To include third-party modules within the embedded interpreter's environment, use the `search_paths`
entry within the `python` table:
```toml
[python]
search_paths = [
    "/home/user/venvs/exa/lib/python3.12/site-packages",
]
```
Note that external libraries **must** match the Python version of the application (currently 3.12).
Libraries should be set up externally via `venv`/`pip` or some other Python environment management
tool(s).


## Data Files
Data is saved using the [HDF5 file format](https://www.hdfgroup.org/solutions/hdf5/). If enabled,
a new data file will be generated each run containing datasets for each plot. Calls to `plot()`
will append new data to the respective dataset (unless the `write` argument is `False`, e.g.
`plot[1](x, y, write=False)`). For each plot, the data file will have two datasets corresponding to
each plot type (so for each plot, there will be a dataset for 2D plot data and a dataset for
colormap data).

Dataset names are formed by the following convention:
```
dataset<#>.<type>
```
where `<#>` corresponds to the plot ID, and `<type>` is the plot type (either `twodimen` or
`colormap`). So, for example, the 2D dataset of plot 1 would have the name `dataset1.twodimen`.

Anything that can read HDF5 files should be able to provide access to the data. For example, we can
use the [HDF5 Python library](https://docs.h5py.org/en/stable/):
```python
import h5py

with h5py.File('data.hdf5', 'r') as f:
    dataset = f['dataset1.twodimen']
    ...
```


## Further Reading
API documentation can be found [here](docs/API.md) (and similarly in the library's
[stub file](python/exaplot.pyi)). Several example scripts are also provided as a reference.
