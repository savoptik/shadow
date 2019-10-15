
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

chage_LDADD    = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX)
newuidmap_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
newgidmap_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
chfn_LDADD     = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT_NOPAM) $(LIBSKEY) $(LIBMD)
chgpasswd_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT)
chsh_LDADD     = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT_NOPAM) $(LIBSKEY) $(LIBMD)
chpasswd_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) -lpam_userpass
gpasswd_LDADD  = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT)
groupadd_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX)
groupdel_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX)
groupmems_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX)
groupmod_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX)
grpck_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
grpconv_LDADD  = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
grpunconv_LDADD = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
lastlog_LDADD   = $(LDADD) $(LIBAUDIT)
newgrp_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBCRYPT)
newusers_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) -lpam_userpass
nologin_LDADD  =
pwck_LDADD     = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
pwconv_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
pwunconv_LDADD = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)
sulogin_LDADD  = $(LDADD) $(LIBCRYPT)
useradd_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBACL) $(LIBATTR)
userdel_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE)
usermod_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBACL) $(LIBATTR)
vipw_LDADD     = $(LDADD) $(LIBAUDIT) $(LIBSELINUX)

install-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
	ln -sf newgrp	$(DESTDIR)$(ubindir)/sg
	ln -sf vipw	$(DESTDIR)$(usbindir)/vigr
	for i in $(suidbins); do \
		chmod $(suidperms) $(DESTDIR)$(bindir)/$$i; \
	done
	for i in $(suidubins); do \
		chmod $(suidperms) $(DESTDIR)$(ubindir)/$$i; \
	done
if WITH_TCB
	for i in $(shadowsgidubins); do \
		chown root:shadow $(DESTDIR)$(ubindir)/$$i; \
		chmod $(sgidperms) $(DESTDIR)$(ubindir)/$$i; \
	done
endif
