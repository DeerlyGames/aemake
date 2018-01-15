#!/bin/sh
LD_LIBRARY_PATH=/home/michael/Downloads/qt-everywhere-opensource-src-5.9.3/qtbase/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
QT_PLUGIN_PATH=/home/michael/Downloads/qt-everywhere-opensource-src-5.9.3/qtbase/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec /home/michael/Downloads/qt-everywhere-opensource-src-5.9.3/qtbase/bin/qdbusxml2cpp "$@"
