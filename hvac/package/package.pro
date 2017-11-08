
DISTFILES = icon.svg config.xml

copy_icon.target = $$OUT_PWD/root/icon.svg
copy_icon.depends = $$_PRO_FILE_PWD_/icon.svg
copy_icon.commands = $(COPY_FILE) \"$$replace(copy_icon.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_icon.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_icon
PRE_TARGETDEPS += $$copy_icon.target

copy_config.target = $$OUT_PWD/root/config.xml
copy_config.depends = $$_PRO_FILE_PWD_/config.xml
copy_config.commands = $(COPY_FILE) \"$$replace(copy_config.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_config.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_config
PRE_TARGETDEPS += $$copy_config.target

wgt.target = package
wgt.commands = wgtpkg-pack -f -o hvac.wgt root

QMAKE_EXTRA_TARGETS += wgt
