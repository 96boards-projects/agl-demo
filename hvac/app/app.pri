TEMPLATE = app

load(configure)
qtCompileTest(libhomescreen)

config_libhomescreen {
    CONFIG += link_pkgconfig
    PKGCONFIG += homescreen
    DEFINES += HAVE_LIBHOMESCREEN
}

DESTDIR = $${OUT_PWD}/../package/root/bin
