
AUTOMAKE_OPTIONS = 1.0 foreign

DEFS = 

noinst_LTLIBRARIES = libshadow.la

libshadow_la_LDFLAGS = -version-info 0:0:0
libshadow_la_CPPFLAGS = $(ECONF_CPPFLAGS)
if HAVE_VENDORDIR
libshadow_la_CPPFLAGS += -DVENDORDIR=\"$(VENDORDIR)\"
endif

libshadow_la_SOURCES = \
	commonio.c \
	commonio.h \
	defines.h \
	encrypt.c \
	exitcodes.h \
	faillog.h \
	fields.c \
	fputsx.c \
	getdef.c \
	getdef.h \
	get_gid.c \
	getlong.c \
	get_pid.c \
	get_uid.c \
	getulong.c \
	groupio.c \
	groupmem.c \
	groupio.h \
	gshadow.c \
	lockpw.c \
	nscd.c \
	nscd.h \
	sssd.c \
	sssd.h \
	pam_defs.h \
	port.c \
	port.h \
	prototypes.h \
	pwauth.c \
	pwauth.h \
	pwio.c \
	pwio.h \
	pwmem.c \
	subordinateio.h \
	subordinateio.c \
	selinux.c \
	semanage.c \
	sgetgrent.c \
	sgetpwent.c \
	sgetspent.c \
	sgroupio.c \
	sgroupio.h\
	shadow.c \
	shadowio.c \
	shadowio.h \
	shadowmem.c \
	spawn.c \
	utent.c

if WITH_TCB
libshadow_la_SOURCES += tcbfuncs.c tcbfuncs.h
endif

semanage_filter = sed -n 's/.*\(semanage_[[:alpha:]_]\+\)[[:space:]]*(.*/\1/p'

ref.list: semanage.c
	$(semanage_filter) $^ | sort -u > $@

def.list:
	echo '#include <semanage/semanage.h>' |\
		$(CPP) $(CPPFLAGS) -xc - |\
		$(semanage_filter) | sort -u > $@

sym.list: ref.list def.list
	comm -12 $^ > sym.list

redir_sym.h: sym.list
	sed 's/.*/#define & (*&)/' $^ > $@

redef_sym.h: sym.list
	sed 's/.*/#undef &\n__typeof__(&) &;/' $^ > $@

res_sym.h: sym.list
	sed 's/.*/\&\& !!(& = dlsym(dlhandle, "&"))/' $^ > $@

if WITH_SELINUX
BUILT_SOURCES = redir_sym.h redef_sym.h res_sym.h
endif

# These files are unneeded for some reason, listed in
# order of appearance:
#
# sources for dbm support (not yet used)

EXTRA_DIST = \
	.indent.pro \
	gshadow_.h
