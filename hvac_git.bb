SUMMARY     = "HVAC demo for Dragonboard410c"
DESCRIPTION = "AGL UI and binding to display HVAC data"
HOMEPAGE    = "https://github.com/Mani-Sadhasivam/hvac.git"

SECTION     = "apps"

LICENSE     = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ae6497158920d9524cf208c09cc4c984"

SRC_URI = "git://github.com/Mani-Sadhasivam/agl;protocol=git;"
SRCREV  = "12a6108313425b8dbe5da716b6e852a3aad83ea7"

PV = "4.0+git${SRCPV}"
S  = "${WORKDIR}/git/hvac"

# build-time dependencies
DEPENDS += "qtquickcontrols2"

inherit qmake5 aglwgt
