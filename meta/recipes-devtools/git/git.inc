SUMMARY = "Distributed version control system"
HOMEPAGE = "http://git-scm.com"
SECTION = "console/utils"
LICENSE = "GPLv2"
DEPENDS = "openssl curl zlib expat"

PROVIDES_append_class-native = " git-replacement-native"

SRC_URI = "${KERNELORG_MIRROR}/software/scm/git/git-${PV}.tar.gz;name=tarball \
           ${KERNELORG_MIRROR}/software/scm/git/git-manpages-${PV}.tar.gz;name=manpages"

S = "${WORKDIR}/git-${PV}"

LIC_FILES_CHKSUM = "file://COPYING;md5=7c0d7ef03a7eb04ce795b0f60e68e7e1"

EXTRA_OECONF = "--with-perl=${STAGING_BINDIR_NATIVE}/perl-native/perl \
		--without-tcltk \
"

# Needs brokensep as this doesn't use automake
inherit autotools-brokensep perlnative bash-completion

EXTRA_OEMAKE = "NO_PYTHON=1 CFLAGS='${CFLAGS}' LDFLAGS='${LDFLAGS}'"
EXTRA_OEMAKE += "'PERL_PATH=/usr/bin/env perl'"
EXTRA_OEMAKE_append_class-native = " NO_CROSS_DIRECTORY_HARDLINKS=1"

do_compile_prepend () {
	# Remove perl/perl.mak to fix the out-of-date perl.mak error
	# during rebuild
	rm -f perl/perl.mak
}

do_install () {
	oe_runmake install DESTDIR="${D}" bindir=${bindir} \
		template_dir=${datadir}/git-core/templates
	install -d ${D}/${mandir}/man1
	install -d ${D}/${mandir}/man5
	install -d ${D}/${mandir}/man7
	install -t ${D}/${mandir}/man1 ${WORKDIR}/man1/*
	install -t ${D}/${mandir}/man5 ${WORKDIR}/man5/*
	install -t ${D}/${mandir}/man7 ${WORKDIR}/man7/*
	install -d ${D}/${datadir}/bash-completion/completions/
	install -m 644 ${S}/contrib/completion/git-completion.bash ${D}/${datadir}/bash-completion/completions/git
}

perl_native_fixup () {
	sed -i -e 's#${STAGING_BINDIR_NATIVE}/perl-native/#${bindir}/#' \
	       -e 's#${libdir}/perl-native/#${libdir}/#' \
	    ${@d.getVar("PERLTOOLS").replace(' /',d.getVar('D') + '/')}

	# ${libdir} is not applicable here, perl-native files are always
	# installed to /usr/lib on both 32/64 bits targets.

	mkdir -p ${D}${libdir}
	mv ${D}${exec_prefix}/lib/perl-native/perl ${D}${libdir}
	rmdir -p ${D}${exec_prefix}/lib/perl-native || true
}

REL_GIT_EXEC_PATH = "${@os.path.relpath(libexecdir, bindir)}/git-core"
REL_GIT_TEMPLATE_DIR = "${@os.path.relpath(datadir, bindir)}/git-core/templates"

do_install_append_class-target () {
	perl_native_fixup
}

do_install_append_class-native() {
	create_wrapper ${D}${bindir}/git \
		GIT_EXEC_PATH='`dirname $''realpath`'/${REL_GIT_EXEC_PATH} \
		GIT_TEMPLATE_DIR='`dirname $''realpath`'/${REL_GIT_TEMPLATE_DIR}
}

do_install_append_class-nativesdk() {
	create_wrapper ${D}${bindir}/git \
		GIT_EXEC_PATH='`dirname $''realpath`'/${REL_GIT_EXEC_PATH} \
		GIT_TEMPLATE_DIR='`dirname $''realpath`'/${REL_GIT_TEMPLATE_DIR}
	perl_native_fixup
}

FILES_${PN} += "${datadir}/git-core ${libexecdir}/git-core/"

PERLTOOLS = " \
    ${libexecdir}/git-core/git-add--interactive \
    ${libexecdir}/git-core/git-archimport \
    ${libexecdir}/git-core/git-cvsexportcommit \
    ${libexecdir}/git-core/git-cvsimport \
    ${libexecdir}/git-core/git-cvsserver \
    ${bindir}/git-cvsserver \
    ${libexecdir}/git-core/git-difftool \
    ${libexecdir}/git-core/git-relink \
    ${libexecdir}/git-core/git-send-email \
    ${libexecdir}/git-core/git-svn \
    ${libexecdir}/git-core/git-instaweb \
    ${libexecdir}/git-core/git-submodule \
    ${libexecdir}/git-core/git-am \
    ${libexecdir}/git-core/git-request-pull \
    ${datadir}/gitweb/gitweb.cgi \
    ${datadir}/git-core/templates/hooks/prepare-commit-msg.sample \
    ${datadir}/git-core/templates/hooks/pre-rebase.sample \
"

# Git tools requiring perl
PACKAGES =+ "${PN}-perltools"
FILES_${PN}-perltools += " \
    ${PERLTOOLS} \
    ${libdir}/perl \
    ${datadir}/perl \
"
RDEPENDS_${PN}-perltools = "${PN} perl perl-module-file-path findutils"

# git-tk package with gitk and git-gui
PACKAGES =+ "${PN}-tk"
#RDEPENDS_${PN}-tk = "${PN} tk tcl"
#EXTRA_OEMAKE = "TCL_PATH=${STAGING_BINDIR_CROSS}/tclsh"
FILES_${PN}-tk = " \
    ${bindir}/gitk \
    ${datadir}/gitk \
"

PACKAGES =+ "gitweb"
FILES_gitweb = "${datadir}/gitweb/"


BBCLASSEXTEND = "native nativesdk"
