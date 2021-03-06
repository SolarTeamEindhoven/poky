SUMMARY = "Wayland, a protocol between a compositor and clients"
DESCRIPTION = "Wayland is a protocol for a compositor to talk to its clients \
as well as a C library implementation of that protocol. The compositor can be \
a standalone display server running on Linux kernel modesetting and evdev \
input devices, an X application, or a wayland client itself. The clients can \
be traditional applications, X servers (rootless or fullscreen) or other \
display servers."
HOMEPAGE = "http://wayland.freedesktop.org"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=b31d8f53b6aaf2b4985d7dd7810a70d1 \
                    file://src/wayland-server.c;endline=24;md5=b8e046164a766bb1ede8ba38e9dcd7ce"

DEPENDS = "expat libffi wayland-native"

SRC_URI = "https://wayland.freedesktop.org/releases/${BPN}-${PV}.tar.xz \
           "
SRC_URI[md5sum] = "ee615e2d1e205eec48b02f069327eb96"
SRC_URI[sha256sum] = "d6b4135cba0188abcb7275513c72dede751d6194f6edc5b82183a3ba8b821ab1"

inherit autotools pkgconfig

PACKAGECONFIG ?= "dtdvalidation hostscanner libraries"
PACKAGECONFIG_class-native ?= "dtdvalidation"
PACKAGECONFIG_class-nativesdk ?= "dtdvalidation hostscanner libraries"

PACKAGECONFIG[libraries] = "--enable-libraries,--disable-libraries"
PACKAGECONFIG[doc] = "--enable-documentation,--disable-documentation"
PACKAGECONFIG[hostscanner] = "--with-host-scanner,--without-host-scanner"
PACKAGECONFIG[dtdvalidation] = "--enable-dtd-validation,--disable-dtd-validation,libxml2"

# Wayland installs a M4 macro for other projects to use, which uses the target
# pkg-config to find files.  Replace pkg-config with pkg-config-native.
do_install_append_class-native() {
  sed -e 's,PKG_CHECK_MODULES(.*),,g' \
      -e 's,$PKG_CONFIG,pkg-config-native,g' \
      -i ${D}/${datadir}/aclocal/wayland-scanner.m4
}

sysroot_stage_all_append_class-target () {
	rm ${SYSROOT_DESTDIR}/${datadir}/aclocal/wayland-scanner.m4
	cp ${STAGING_DATADIR_NATIVE}/aclocal/wayland-scanner.m4 ${SYSROOT_DESTDIR}/${datadir}/aclocal/
}

FILES_${PN} = "${libdir}/*${SOLIBS}"
FILES_${PN}-dev += "${bindir} ${datadir}/wayland"

BBCLASSEXTEND = "native nativesdk"
