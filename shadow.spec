Name: shadow
Version: 4.0.0
Release: alt15
Serial: 1

%define BUILD_LIBSHADOW 0
%if %BUILD_LIBSHADOW
%define	enable_shared enable-shared
%else
%define	enable_shared disable-shared
%endif

Summary: Utilities for managing shadow password files and user/group accounts
License: BSD
Group: System/Base
Url: ftp://ftp.pld.org.pl/software/shadow

Source0: %url/%name-%version.tar.bz2
Source1: login.defs
Source2: useradd.default
Source3: user-group-mod.pamd
Source4: chage-chfn-chsh.pamd
Source5: chpasswd-newusers.pamd
Source6: chage.control
Source7: chfn.control
Source8: chsh.control
Source9: gpasswd.control
Source10: newgrp.control

# Owl
Patch0: shadow-4.0.0-owl-warnings.patch
Patch1: shadow-4.0.0-owl-alt-check-reads.patch
Patch2: shadow-4.0.0-owl-usermod-unlock.patch
Patch3: shadow-4.0.0-owl-tmp.patch
Patch4: shadow-4.0.0-owl-pam-auth.patch
Patch5: shadow-4.0.0-owl-chage-drop-priv.patch
Patch6: shadow-4.0.0-owl-chage-ro-no-lock.patch
Patch7: shadow-4.0.0-owl-useradd-usermod-usage.patch
Patch8: shadow-4.0.0-owl-pam_chauthtok.patch
Patch10: shadow-4.0.0-rh-owl-redhat.patch
Patch20: shadow-4.0.0-owl-man.patch
Patch21: shadow-4.0.0-alt-check_names.patch
Patch22: shadow-4.0.0-owl-create-mailbox.patch
Patch23: shadow-4.0.0-owl-restrict-locale.patch
Patch24: shadow-4.0.0-owl-crypt_gensalt.patch
Patch25: shadow-4.0.0-owl-newgrp.patch
Patch26: shadow-4.0.0-owl-automake.patch
Patch30: shadow-4.0.0-owl-alt-tcb.patch

# ALT
Patch100: shadow-4.0.0-alt-default_skel.patch
Patch101: shadow-4.0.0-alt-progname.patch
Patch102: shadow-4.0.0-alt-configure-fix.patch
Patch103: shadow-4.0.0-alt-disable-build-unused.patch
Patch104: shadow-4.0.0-alt-fix-userdel-path_prefix.patch
Patch105: shadow-4.0.0-alt-skel.patch
Patch106: shadow-4.0.0-alt-copy_tree-perms.patch
Patch107: shadow-4.0.0-alt-user_groups.patch
Patch108: shadow-4.0.0-alt-configure-passwd.patch
Patch109: shadow-4.0.0-alt-configure-gettext.patch

BuildPreReq: mktemp >= 1:1.3.1, rpm-build >= 4.0.4-alt10

# Automatically added by buildreq on Mon Oct 28 2002
BuildRequires: cvs libpam-devel libtcb-devel pam_userpass-devel

%description
This package includes the tools necessary for manipulating local user and
group databases. It supports both traditional and tcb shadow password files.

%package -n lib%name
Summary: Shadow password file routines library
Group: System/Libraries

%description -n lib%name
Shadow library manipulates local user and group databases. It supports both
traditional and tcb shadow password files.
This package contains shared library required for various shadow utils.

%package -n lib%name-devel
Summary: Development files for the shadow password file routines library
Group: Development/C
PreReq: lib%name = %serial:%version-%release

%description -n lib%name-devel
Shadow library manipulates local user and group databases. It supports both
traditional and tcb shadow password files.
This package contains files required for development software based
on lib%name.

%package -n lib%name-devel-static
Summary: Shadow password file routines static library
Group: Development/C
PreReq: lib%name-devel = %serial:%version-%release

%description -n lib%name-devel-static
Shadow library manipulates local user and group databases. It supports both
traditional and tcb shadow password files.
This package contains static library required for development statically
linked software based on lib%name.

%package utils
Summary: Utilities for managing shadow password files and user/group accounts
Group: System/Base
PreReq: %name-convert = %serial:%version-%release, tcb-utils >= 0.9.8
%if !%BUILD_LIBSHADOW
#Obsoletes: lib%name, lib%name-devel, lib%name-devel-static
%endif
Obsoletes: adduser

%description utils
This package includes utilities for managing shadow password files and
user/group accounts:
+ useradd: creates a new user or updates default new user information;
+ userdel: deletes a user account and related files;
+ usermod: modifies a user account;
+ groupadd: creates a new group;
+ groupdel: deletes a group;
+ groupmod: modifies a group;
+ newusers: updates and creates new users in batch;
+ chpasswd: updates password file in batch.

%package check
Summary: Utilities for checking integrity of the password, group, shadow-password, or shadow-group files
Group: System/Base
PreReq: %name-convert = %serial:%version-%release

%description check
This package includes utilities for checking integrity of the password, group,
shadow-password, or shadow-group files:
+ pwck: verifies the integrity of the system password authentication information;
+ grpck: verifies the integrity of the system group authentication information.

%package convert
Summary: Utilities for convertion to and from shadow passwords and groups
Group: System/Base
%if %BUILD_LIBSHADOW
PreReq: lib%name = %serial:%version-%release
%endif

%description convert
This package includes utilities for convertion to and from shadow passwords
and groups:
+ pwconv: creates shadow from passwd and an optionally existing shadow;
+ pwunconv: creates passwd from passwd and shadow and then removes shadow;
+ grpconv: creates gshadow from group and an optionally existing gshadow;
+ grpunconv: creates group from group and gshadow and then removes gshadow.

%package change
Summary: Utilities for changing user shell, finger and password information
Group: System/Base
PreReq: %name-utils = %serial:%version-%release, control

%description change
This package includes utilities for changing user shell, finger and password
information:
+ chage: changes the number of days between password changes and the date of
         the last password change;
+ chfn: changes user fullname, office number, office extension, and home phone
        number information for a user's account;
+ chsh: changes the user login shell.

%package edit
Summary: Utilities for editing the password, group, shadow-password, or shadow-group files
Group: System/Base
PreReq: %name-utils = %serial:%version-%release

%description edit
This package includes utilities for editing the password, group,
shadow-password, or shadow-group files:
+ vipw: edits the /etc/passwd and /etc/shadow files;
+ vigr: edits the /etc/group and /etc/gshadow files.

%package groups
Summary: Utilities for execute command as different group ID
Group: System/Base
PreReq: %name-utils = %serial:%version-%release, control

%description groups
This package includes utilities for execute command as different group ID:
+ gpasswd: is used to administer the /etc/group and etc/gshadow files;
+ newgrp: is used to change the current group ID during a login session;
+ sg: is used to execute command as different group ID.

%package log
Summary: Utilities for examining lastlog and faillog files
Group: System/Base
PreReq: %name-utils = %serial:%version-%release

%description log
This package includes utilities for examining lastlog and faillog files:
+ faillog: formats the contents of the system failure log file, and maintains
           failure counts and limits;
+ lastlog: formats the contents of the system last login file.

%prep
%setup -q

# Owl
%patch0 -p1
%patch1 -p1
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1
%patch7 -p1
%patch8 -p1
%patch10 -p1
%patch20 -p1
%patch21 -p1
%patch22 -p1
%patch23 -p1
%patch24 -p1
%patch25 -p1
%patch26 -p1
%patch30 -p1

# ALT
%patch100 -p1
%patch101 -p1
%patch102 -p1
%patch103 -p1
%patch104 -p1
%patch105 -p1
%patch106 -p1
%patch107 -p1
%patch108 -p1
%patch109 -p1

find -type f -name \*.orig -print -delete

%build
# buildreq hangs on some checks.
%{?__buildreqs:export gt_cv_int_divbyzero_sigfpe=yes}

find lib libmisc src -type f -name \*.c >po/POTFILES.in
rm -rf intl
%__install -pv -m644 /usr/share/gettext/intl/Makevars* po/Makevars
autoreconf -fisv
%__subst 's/^\(mkinstalldirs =\).*/\1 $(SHELL) $(MKINSTALLDIRS)/' po/Makefile*

%add_optflags -DEXTRA_CHECK_HOME_DIR
%configure \
	--%enable_shared \
	--enable-static \
	--disable-desrpc \
	--with-libcrypt \
	--with-libpam \
	--without-libcrack
make

%install
%makeinstall

%__install -pD -m640 %SOURCE1 $RPM_BUILD_ROOT%_sysconfdir/login.defs
%__install -pD -m600 %SOURCE2 $RPM_BUILD_ROOT%_sysconfdir/default/useradd

%__mkdir_p $RPM_BUILD_ROOT%_sysconfdir/pam.d
pushd $RPM_BUILD_ROOT%_sysconfdir/pam.d
%__install -p -m600 $RPM_SOURCE_DIR/user-group-mod.pamd user-group-mod
%__ln_s user-group-mod groupadd
%__ln_s user-group-mod groupdel
%__ln_s user-group-mod groupmod
%__ln_s user-group-mod useradd
%__ln_s user-group-mod userdel
%__ln_s user-group-mod usermod
%__install -p -m640 $RPM_SOURCE_DIR/chage-chfn-chsh.pamd chage-chfn-chsh
%__ln_s chage-chfn-chsh chage
%__ln_s chage-chfn-chsh chfn
%__ln_s chage-chfn-chsh chsh
%__install -p -m600 $RPM_SOURCE_DIR/chpasswd-newusers.pamd chpasswd-newusers
%__ln_s chpasswd-newusers chpasswd
%__ln_s chpasswd-newusers newusers
popd

%__ln_s useradd $RPM_BUILD_ROOT%_sbindir/adduser
%__ln_s vipw $RPM_BUILD_ROOT%_sbindir/vigr
%__ln_s vipw.8 $RPM_BUILD_ROOT%_man8dir/vigr.8

for n in getspent getspnam setspent endspent fgetspent sgetspent putspent lckpwdf ulckpwdf; do
	%__ln_s shadow.3 "$RPM_BUILD_ROOT%_man3dir/$n"
done

%__install -pD -m755 $RPM_SOURCE_DIR/chage.control $RPM_BUILD_ROOT/etc/control.d/facilities/chage
%__install -pD -m755 $RPM_SOURCE_DIR/chfn.control $RPM_BUILD_ROOT/etc/control.d/facilities/chfn
%__install -pD -m755 $RPM_SOURCE_DIR/chsh.control $RPM_BUILD_ROOT/etc/control.d/facilities/chsh
%__install -pD -m755 $RPM_SOURCE_DIR/gpasswd.control $RPM_BUILD_ROOT/etc/control.d/facilities/gpasswd
%__install -pD -m755 $RPM_SOURCE_DIR/newgrp.control $RPM_BUILD_ROOT/etc/control.d/facilities/newgrp

%find_lang %name

%post -n lib%name -p %post_ldconfig
%postun -n lib%name -p %postun_ldconfig

%post convert
if [ $1 = 1 ]; then
	if [ ! -e /etc/gshadow ]; then
		%_sbindir/grpconv
	fi
	if [ ! -e /etc/shadow -a ! -e /etc/tcb ]; then
		%_sbindir/pwconv
	fi
fi

%pre change
[ $1 -eq 1 ] || /usr/sbin/control-dump chage chfn chsh

%post change
[ $1 -eq 1 ] || /usr/sbin/control-restore chage chfn chsh

%pre groups
[ $1 -eq 1 ] || /usr/sbin/control-dump gpasswd newgrp

%post groups
[ $1 -eq 1 ] || /usr/sbin/control-restore gpasswd newgrp

%if %BUILD_LIBSHADOW
%files -n lib%name
%_libdir/*.so*

%files -n lib%name-devel
%_libdir/*.so
%_libdir/*.la
%_man3dir/*

%files -n lib%name-devel-static
%_libdir/*.a
%endif

%files utils -f %name.lang
%attr(751,root,root) %dir %_sysconfdir/default
%attr(600,root,root) %config(noreplace) %_sysconfdir/default/useradd
%attr(640,root,shadow) %config(noreplace) %_sysconfdir/login.defs
%config(noreplace) %_sysconfdir/pam.d/user-group-mod
%_sysconfdir/pam.d/groupadd
%_sysconfdir/pam.d/groupdel
%_sysconfdir/pam.d/groupmod
%_sysconfdir/pam.d/useradd
%_sysconfdir/pam.d/userdel
%_sysconfdir/pam.d/usermod
%config(noreplace) %_sysconfdir/pam.d/chpasswd-newusers
%_sysconfdir/pam.d/chpasswd
%_sysconfdir/pam.d/newusers
%_sbindir/user*
%_sbindir/group*
%_sbindir/adduser
%_sbindir/newusers
%_sbindir/chpasswd
%_mandir/man?/login.defs.*
%_mandir/man?/adduser.*
%_mandir/man?/group*.*
%_mandir/man?/user*.*
%_mandir/man?/newusers.*
%_mandir/man?/chpasswd.*
#%_man5dir/shadow.*
%doc ChangeLog NEWS README TODO doc/{ANNOUNCE,LICENSE}
#%doc doc/{ANNOUNCE,CHANGES,HOWTO,LICENSE,README,README.linux}

%files check
%_sbindir/*ck
%_mandir/man?/*ck.*

%files convert
%_sbindir/*conv
%_mandir/man?/*conv.*

%files change
%config /etc/control.d/facilities/chage
%config /etc/control.d/facilities/chfn
%config /etc/control.d/facilities/chsh
%attr(640,root,shadow) %config(noreplace) %_sysconfdir/pam.d/chage-chfn-chsh
%_sysconfdir/pam.d/chage
%_sysconfdir/pam.d/chfn
%_sysconfdir/pam.d/chsh
%attr(2711,root,shadow) %_bindir/chage
%attr(4711,root,root) %_bindir/chfn
%attr(4711,root,root) %_bindir/chsh
%_mandir/man?/chage.*
%_mandir/man?/chfn.*
%_mandir/man?/chsh.*

%files edit
%_sbindir/vi??
%_mandir/man?/vi??.*

%files groups
%config /etc/control.d/facilities/gpasswd
%config /etc/control.d/facilities/newgrp
%attr(4711,root,root) %_bindir/gpasswd
%attr(4711,root,root) %_bindir/newgrp
%_bindir/sg
%_mandir/man?/gpasswd.*
%_mandir/man?/newgrp.*
%_mandir/man?/sg.*

%files log
%_bindir/*log
%_mandir/man?/*log.*

%changelog
* Thu Jun 10 2004 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt15
- Properly check the return value from pam_chauthtok() in
  libmisc/pwdcheck.c: passwd_check() that is used by chfn and
  chsh commands (Owl).
  Thanks to Steve Grubb, Martin Schulze and Solar Designer.

* Thu Mar 25 2004 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt14
- Fixed build with new gettext and autotools.
- Fixed typo in chage-chfn-chsh.pamd (#3904).

* Sat Nov 22 2003 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt13
- In tcbfuncs/tcb_move(), use mode 0700 instead of mode 0 for the
  directory being modified as the latter is incompatible with
  the mode 0 hack in vserver kernel patches.

* Wed Aug 20 2003 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt12
- Explicitly use old libtool for build.

* Mon Jun 30 2003 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt11
- useradd, usermod:
  fixed user_group initialization (voins, #0001875).

* Sat May 24 2003 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt10
- PAM configuration policy enforcement.

* Sat Apr 12 2003 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt9
- Rebuilt with libpam_userpass.so.1.

* Mon Oct 28 2002 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt8
- Merged Owl changes:
  * Thu Oct 24 2002 Solar Designer <solar@owl.openwall.com>
  - Cleaned up the recent changes.
  - Corrected a newly introduced memory leak on an error path.
  - Changed the TCB_SYMLINKS pseudo-code in login.defs(5) manual page to be
    C/English rather than shell for consistency with the pam_tcb(8) page.
  * Mon Aug 19 2002 Rafal Wojtczuk <nergal@owl.openwall.com>
  - Merged the enhancements which remove 32K users limit.

* Thu Oct 17 2002 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt7
- Added control support for chage, chfn, chsh, gpasswd, and newgrp.

* Wed Aug 14 2002 Dmitry V. Levin <ldv@altlinux.org> 1:4.0.0-alt6
- copy_tree: ensure strict permissions of created files.
- chage: made "chage -l" drop its saved GID too (Owl).
- useradd, usermod: removed the extra space in "[-e expire ]" in the usage instructions (Owl).

* Mon Mar 18 2002 Dmitry V. Levin <ldv@alt-linux.org> 1:4.0.0-alt5
- Updated chkname patch.

* Fri Jan 25 2002 Stanislav Ievlev <inger@altlinux.ru> 1:4.0.0-alt4
- added rollback to standart skeleton dir if it doesn't exits

* Fri Dec 21 2001 Dmitry V. Levin <ldv@alt-linux.org> 1:4.0.0-alt3
- def_load: don't exit when /etc/login.defs not available.

* Thu Dec 20 2001 Dmitry V. Levin <ldv@alt-linux.org> 1:4.0.0-alt2
- userdel: fixed long standing bug in path_prefix check.

* Tue Dec 18 2001 Dmitry V. Levin <ldv@alt-linux.org> 1:4.0.0-alt1
- 4.0.0
- Merged in 16 patches from Owl.
- Updated default_skel and progname patches (all the rest are obsolete).
- Disabled build of unused software.
- Changed interpackage dependencies.
- %name-convert: convert group and passwd files after first install.
- Disabled libshadow.

* Mon Sep 10 2001 Dmitry V. Levin <ldv@altlinux.ru> 20000902-alt3
- Fixed typo in mailspool patch.
- Added %%post scripts to ease migration.

* Mon Aug 13 2001 Dmitry V. Levin <ldv@altlinux.ru> 20000902-alt2
- Split shadow-utils into several subpackages.
- Libification.
- Remade mailspool patch (new options: z,Z,K).
- Enable packaging of chsh, chfn, vipw, vigr, newgrp.

* Thu Aug 02 2001 Dmitry V. Levin <ldv@altlinux.ru> 20000902-alt1
- 20000902
- Merged RH (up to 20000902-3) and Owl (up to 19990827-16owl) patches and configs.
- Get rid of %_sbindir/{d,mk}passwd and its manpages.

* Sun Feb 25 2001 Dmitry V. Levin <ldv@fandra.org> 20000826-ipl1mdk
- 20000826
- Merged MDK patches.
- Added progname patch.

* Sun Nov 05 2000 Dmitry V. Levin <ldv@fandra.org> 19990827-ipl9mdk
- Merge RH patches.
- FHSification.

* Mon May 29 2000 Dmitry V. Levin <ldv@fandra.org> 19990827-ipl8mdk
- Fix: updated docs about -D -k option.
- RE and Fandra adaptions.

* Fri Dec 3 1999 Florent Villard <warly@mandrakesoft.com>
- correct a segfault problem with NIS

* Sat Nov 13 1999 AEN <aen@logic.ru>
- Feature: added -D -k option.

* Wed Sep 22 1999 Cristian Gafton <gafton@redhat.com>
- fix segfault for userdel when the primary group for the user is not defined

* Tue Sep 21 1999 Cristian Gafton <gafton@redhat.com>
- Serial: 1 because now we are using 19990827 (why the heck can't they have
  a normal version just like everybody else?!)
- ported all patches to the new code base

* Thu Apr 15 1999 Bill Nottingham <notting@redhat.com>
- SIGHUP nscd from usermod, too

* Fri Apr 09 1999 Michael K. Johnson <johnsonm@redhat.com>
- added usermod password locking from Chris Adams <cadams@ro.com>

* Thu Apr 08 1999 Bill Nottingham <notting@redhat.com>
- have things that modify users/groups SIGHUP nscd on exit

* Wed Mar 31 1999 Michael K. Johnson <johnsonm@redhat.com>
- have userdel remove user private groups when it is safe to do so
- allow -f to force user removal even when user appears busy in utmp

* Tue Mar 23 1999 Preston Brown <pbrown@redhat.com>
- edit out unused CHFN fields from login.defs.

* Sun Mar 21 1999 Cristian Gafton <gafton@redhat.com>
- auto rebuild in the new build environment (release 7)

* Wed Jan 13 1999 Bill Nottingham <notting@redhat.com>
- configure fix for arm

* Wed Dec 30 1998 Cristian Gafton <gafton@redhat.com>
- build against glibc 2.1

* Fri Aug 21 1998 Jeff Johnson <jbj@redhat.com>
- Note that /usr/sbin/mkpasswd conflicts with /usr/bin/mkpasswd;
  one of these (I think /usr/sbin/mkpasswd but other opinions are valid)
  should probably be renamed.  In any case, mkpasswd.8 from this package
  needs to be installed. (problem #823)

* Fri May 08 1998 Prospector System <bugs@redhat.com>
- translations modified for de, fr, tr

* Tue Apr 21 1998 Cristian Gafton <gafton@redhat.com>
- updated to 980403
- redid the patches

* Tue Dec 30 1997 Cristian Gafton <gafton@redhat.com>
- updated the spec file
- updated the patch so that new accounts created on shadowed system won't
  confuse pam_pwdb anymore ('!!' default password instead on '!')
- fixed a bug that made useradd -G segfault
- the check for the ut_user is now patched into configure

* Thu Nov 13 1997 Erik Troan <ewt@redhat.com>
- added patch for XOPEN oddities in glibc headers
- check for ut_user before checking for ut_name -- this works around some
  confusion on glibc 2.1 due to the utmpx header not defining the ut_name
  compatibility stuff. I used a gross sed hack here because I couldn't make
  automake work properly on the sparc (this could be a glibc 2.0.99 problem
  though). The utuser patch works fine, but I don't apply it.
- sleep after running autoconf

* Thu Nov 06 1997 Cristian Gafton <gafton@redhat.com>
- added forgot lastlog command to the spec file

* Mon Oct 26 1997 Cristian Gafton <gafton@redhat.com>
- obsoletes adduser

* Thu Oct 23 1997 Cristian Gafton <gafton@redhat.com>
- modified groupadd; updated the patch

* Fri Sep 12 1997 Cristian Gafton <gafton@redhat.com>
- updated to 970616
- changed useradd to meet RH specs
- fixed some bugs

* Tue Jun 17 1997 Erik Troan <ewt@redhat.com>
- built against glibc
