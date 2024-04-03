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
.\build.ps1
```
