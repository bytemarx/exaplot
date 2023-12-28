## Using QButtonGrid in Qt Designer
To use the `QButtonGrid` widget in Qt Designer, the plugin needs to be moved to the plugins
directory. For example, if using Qt6 Designer on a Debian-based system, the following command
places the plugin in the correct location:
```sh
sudo cp build/app/qbuttongrid/libqbuttongridplugin.so /usr/lib/x86_64-linux-gnu/qt6/plugins/designer/
```
