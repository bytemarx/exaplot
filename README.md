### Supported Platforms
- Linux
- Windows


## Building

Requires:
- CMake 3.22+
- CPython dependencies
- Qt6

### Linux
```sh
cmake -DCMAKE_BUILD_TYPE=Release -B build/
make -C build/
```

### Windows
```powershell
$Env:QT_DIR = "C:\Qt\6.4.0\msvc2019_64"
$Env:MSBUILD = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
.\winbuild.ps1
```


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
