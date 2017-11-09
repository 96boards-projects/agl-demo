SUMMARY     = "HVAC demo for Dragonboard410c"
DESCRIPTION = "AGL UI and binding to display HVAC data"
HOMEPAGE    = "https://github.com/96boards-projects/agl-demo.git"

SECTION     = "apps"

LICENSE     = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ae6497158920d9524cf208c09cc4c984"

SRC_URI = "git://github.com/96boards-projects/agl-demo;protocol=git;"
SRCREV  = "d89be653e40ddd2cfbfd2e597cf030312edcb4c1"

PV = "4.0+git${SRCPV}"
S  = "${WORKDIR}/git/hvac"

# build-time dependencies
DEPENDS += "qtquickcontrols2"

inherit qmake5 aglwgt
