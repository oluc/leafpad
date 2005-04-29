SUBDIRS = src po

desktopdir       = $(datadir)/applications
desktop_in_files = leafpad.desktop.in
desktop_DATA     = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

pixmapsdir       = $(datadir)/pixmaps
pixmaps_DATA     = leafpad.png

EXTRA_DIST = \
	intltool-extract.in intltool-merge.in intltool-update.in \
	$(desktop_DATA) $(desktop_in_files) \
	$(pixmaps_DATA)
