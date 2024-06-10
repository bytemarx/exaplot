cmake -DCMAKE_TOOLCHAIN_FILE="$Env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release -DCMAKE_WIN32_EXECUTABLE=TRUE -B build/
& $Env:MSBUILD .\build\ExaPlot.sln -property:Configuration=Release
& $ENV:QT_DIR\bin\windeployqt.exe .\build\bin\Release\exaplot.exe
