cmake -DCMAKE_BUILD_TYPE=Release -B build/
& $Env:MSBUILD .\build\ZetaPlot.sln -property:Configuration=Release
& $ENV:QT_DIR\bin\windeployqt.exe .\build\bin\Release\zetaplot.exe
