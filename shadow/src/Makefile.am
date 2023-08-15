
EXTRA_DIST = \
	.indent.pro

ubindir = ${prefix}/bin
usbindir = ${prefix}/sbin
suidperms = 4755
sgidperms = 2755

AM_CPPFLAGS = \
	-I${top_srcdir}/lib \
	-I$(top_srcdir)/libmisc \
	-I$(top_srcdir) \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	$(ECONF_CPPFLAGS)

AM_CFLAGS = $(LIBBSD_CFLAGS)

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
ubin_PROGRAMS  = faillog chage chfn chsh expiry gpasswd newgrp groupmems
if ENABLE_SUBIDS
ubin_PROGRAMS += newgidmap newuidmap
endif
if ENABLE_LASTLOG
ubin_PROGRAMS += lastlog
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
		 $(top_builddir)/libmisc/libmisc.la \
		 $(top_builddir)/lib/libshadow.la \
		 $(LIBTCB)

if ACCT_TOOLS_SETUID
LIBPAM_SUID  = $(LIBPAM)
else
LIBPAM_SUID  =
endif

if USE_PAM
LIBCRYPT_NOPAM =
LIBPAM_USERPASS = -lpam_userpass
else
LIBCRYPT_NOPAM = $(LIBCRYPT)
LIBPAM_USERPASS =
endif

chage_LDADD    = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF) -ldl
newuidmap_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCAP) $(LIBECONF) -ldl
newgidmap_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCAP) $(LIBECONF) -ldl
chfn_LDADD     = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT_NOPAM) $(LIBSKEY) $(LIBMD) $(LIBECONF)
chgpasswd_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF)
chsh_LDADD     = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT_NOPAM) $(LIBSKEY) $(LIBMD) $(LIBECONF)
chpasswd_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF) -ldl $(LIBPAM_USERPASS)
expiry_LDADD = $(LDADD) $(LIBECONF)
gpasswd_LDADD  = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF)
groupadd_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF) -ldl
groupdel_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF) -ldl
groupmems_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
groupmod_LDADD = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF) -ldl
grpck_LDADD    = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
grpconv_LDADD  = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
grpunconv_LDADD = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
lastlog_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBECONF)
newgrp_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBCRYPT) $(LIBECONF)
newusers_LDADD = $(LDADD) $(LIBPAM) $(LIBAUDIT) $(LIBSELINUX) $(LIBCRYPT) $(LIBECONF) -ldl $(LIBPAM_USERPASS)
nologin_LDADD  =
pwck_LDADD     = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
pwconv_LDADD   = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
pwunconv_LDADD = $(LDADD) $(LIBAUDIT) $(LIBSELINUX) $(LIBECONF)
sulogin_LDADD  = $(LDADD) $(LIBCRYPT) $(LIBECONF)
useradd_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBACL) $(LIBATTR) $(LIBECONF) -ldl
userdel_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBECONF) -ldl
usermod_LDADD  = $(LDADD) $(LIBPAM_SUID) $(LIBAUDIT) $(LIBSELINUX) $(LIBSEMANAGE) $(LIBACL) $(LIBATTR) $(LIBECONF) -ldl
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

bin_PROGRAMS    +=  getsubids
noinst_PROGRAMS =  get_subid_owners \
					new_subid_range \
					free_subid_range \
					check_subid_range

MISCLIBS = \
	$(LIBAUDIT) \
	$(LIBSELINUX) \
	$(LIBSEMANAGE) \
	$(LIBCRYPT_NOPAM) \
	$(LIBSKEY) \
	$(LIBMD) \
	$(LIBECONF) \
	$(LIBCRYPT) \
	$(LIBTCB)

getsubids_LDADD = \
	$(top_builddir)/lib/libshadow.la \
	$(top_builddir)/libmisc/libmisc.la \
	$(top_builddir)/libsubid/libsubid.la \
	$(MISCLIBS) -ldl

getsubids_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/libmisc \
	-I$(top_srcdir) \
	-I$(top_builddir)/libsubid

get_subid_owners_LDADD = \
	$(top_builddir)/lib/libshadow.la \
	$(top_builddir)/libmisc/libmisc.la \
	$(top_builddir)/libsubid/libsubid.la \
	$(MISCLIBS) -ldl

get_subid_owners_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/libmisc \
	-I$(top_srcdir) \
	-I$(top_builddir)/libsubid

new_subid_range_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/libmisc \
	-I$(top_srcdir) \
	-I$(top_builddir)/libsubid

new_subid_range_LDADD = \
	$(top_builddir)/lib/libshadow.la \
	$(top_builddir)/libmisc/libmisc.la \
	$(top_builddir)/libsubid/libsubid.la \
	$(MISCLIBS) -ldl

free_subid_range_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/libmisc \
	-I$(top_srcdir) \
	-I$(top_builddir)/libsubid

free_subid_range_LDADD = \
	$(top_builddir)/lib/libshadow.la \
	$(top_builddir)/libmisc/libmisc.la \
	$(top_builddir)/libsubid/libsubid.la \
	$(MISCLIBS) -ldl

check_subid_range_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libmisc

check_subid_range_LDADD = \
	$(top_builddir)/lib/libshadow.la \
	$(top_builddir)/libmisc/libmisc.la \
	$(MISCLIBS) -ldl
endif
