TARGET = db410c-temp-binding

SOURCES = db410c-temp-binding.c

LIBS += -Wl,--version-script=$$PWD/export.map

CONFIG += link_pkgconfig
PKGCONFIG += json-c afb-daemon

include(binding.pri)
