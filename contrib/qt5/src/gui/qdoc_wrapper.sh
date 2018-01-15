#!/bin/sh
QT_VERSION=5.9.3
export QT_VERSION
QT_VER=5.9
export QT_VER
QT_VERSION_TAG=593
export QT_VERSION_TAG
QT_INSTALL_DOCS=/home/michael/Downloads/qt-everywhere-opensource-src-5.9.3/qtbase/doc
export QT_INSTALL_DOCS
BUILDDIR=/home/michael/Downloads/qt-everywhere-opensource-src-5.9.3/qtbase/src/gui
export BUILDDIR
exec /usr/local/Qt-5.9.3/bin/qdoc "$@"
