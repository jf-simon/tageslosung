ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
	config.sub configure depcomp install-sh ltmain.sh \
	missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

if HAVE_PO

SUBDIRS += po

endif

EDJE_FLAGS = -v -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = module.desktop e-module-tageslosung.edj

EXTRA_DIST = module.desktop.in \
	e_modules-tageslosung.spec.in \
	e-module-tageslosung.edc \
	$(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-tageslosung.edj module.desktop e_modules-tageslosung.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
