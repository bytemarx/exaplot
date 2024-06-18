$Env:BUILDDIR = "..\..\build"
$Env:ICONFILE = "..\..\app\res\icon.ico"
wix build -arch x64 .\Package.wxs .\Folders.wxs .\AppComponents.wxs .\Package.en-us.wxl -o exaplot-0.9.0.msi
