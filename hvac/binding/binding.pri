TEMPLATE = lib
CONFIG += plugin use_c_linker
CONFIG -= qt
QMAKE_CFLAGS += -Wextra -Wconversion -Wno-unused-parameter -Werror=maybe-uninitialized -Werror=implicit-function-declaration -ffunction-sections -fdata-sections -Wl,--as-needed -Wl,--gc-sections

DESTDIR = $${OUT_PWD}/../package/root/lib
