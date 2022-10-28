
EXTRA_DIST = \
	.indent.pro

ubindir = ${prefix}/bin
usbindir = ${prefix}/sbin
suidperms = 4755
sgidperms = 2755

AM_CPPFLAGS = \
	-I${top_srcdir}/lib \
	-I$(top_srcdir)/libmisc \
	-DLOCALEDIR=\"$(datadir)/locale\"

# XXX why are login and su in /bin anyway (other than for
# historical reasons)?
#
# if the system is screwed so badly that it can't mount /usr,
# you can (hopefully) boot single user, and then you're root
# so you don't need these programs for recovery.
#
# also /lib/libshadow.so.x.xx (if any) could be moved to /usr/lib
# and installation would be much simpler (just two directories,
# $prefix/bin and $prefix/sbin, no install-data hacks...)

bin_PROGRAMS   =
sbin_PROGRAMS  = nologin
ubin_PROGRAMS  = faillog lastlog chage chfn chsh expiry gpasswd newgrp groupmems
if ENABLE_SUBIDS
ubin_PROGRAMS += newgidmap newuidmap
endif
usbin_PROGRAMS = \
	chgpasswd \
	chpasswd \
	groupadd \
	groupdel \
	groupmod \
	grpck \
	grpconv \
	grpunconv \
	logoutd \
	newusers \
	pwck \
	pwconv \
	pwunconv \
	useradd \
	userdel \
	usermod \
	vipw

suidbins       =
suidubins      =

LDADD          = $(INTLLIBS) \
		 $(top_builddir)/libmisc/libmisc.a \
		 $(top_builddir)/lib/libshadow.la \
		 $(LIBTCB)

if ACCT_TOOLS_SETUID
LIBPAM_SUID  = $(LIBPAM)
else
LIBPAM_SUID  =
endif

if USE_PAM
LIBCRYPT_NOPAM =
else
LIBCRYPT_NOPAM = $(LIBCRYPT)
endif

chage_LDADD    = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
newuidmap_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCAP)
newgidmap_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCAP)
chfn_LDADD     = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT_NOPAM) $(LIBSKEY) $(LIBMD) $(LIBECONF)
chgpasswd_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF)
chsh_LDADD     = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT_NOPAM) $(LIBSKEY) $(LIBMD) $(LIBECONF)
chpasswd_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF) -lpam_userpass
expiry_LDADD = $(LDADD) $(LIBECONF)
gpasswd_LDADD  = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF)
groupadd_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
groupdel_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
groupmems_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
groupmod_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
grpck_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
grpconv_LDADD  = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
grpunconv_LDADD = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
lastlog_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBECONF)
newgrp_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBCRYPT) $(LIBECONF)
newusers_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF) -lpam_userpass
nologin_LDADD  =
pwck_LDADD     = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
pwconv_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
pwunconv_LDADD = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
sulogin_LDADD  = $(LDADD) $(LIBCRYPT) $(LIBECONF)
useradd_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBACL) $(LIBATTR) $(LIBECONF)
userdel_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBECONF)
usermod_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBACL) $(LIBATTR) $(LIBECONF)
vipw_LDADD     = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)

install-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
	ln -sf newgrp	$(DESTDIR)$(ubindir)/sg
	ln -sf vipw	$(DESTDIR)$(usbindir)/vigr
	set -e; for i in $(suidbins); do \
		chmod $(suidperms) $(DESTDIR)$(bindir)/$$i; \
	done
	set -e; for i in $(suidubins); do \
		chmod $(suidperms) $(DESTDIR)$(ubindir)/$$i; \
	done
	set -e; for i in $(suidusbins); do \
		chmod $(suidperms) $(DESTDIR)$(usbindir)/$$i; \
	done
if WITH_TCB
	set -e; for i in $(shadowsgidubins); do \
		chown root:shadow $(DESTDIR)$(ubindir)/$$i; \
		chmod $(sgidperms) $(DESTDIR)$(ubindir)/$$i; \
	done
endif
if ENABLE_SUBIDS
if FCAPS
	setcap cap_setuid+ep $(DESTDIR)$(ubindir)/newuidmap
	setcap cap_setgid+ep $(DESTDIR)$(ubindir)/newgidmap
endif
endif
