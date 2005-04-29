bin_PROGRAMS = leafpad

INCLUDES = -DICONDIR=\"$(datadir)/pixmaps\"

leafpad_SOURCES = \
	leafpad.h main.c \
	window.h window.c \
	menu.h menu.c \
	callback.h callback.c \
	view.h view.c \
	undo.h undo.c \
	font.h font.c \
	linenum.h linenum.c \
	indent.h indent.c \
	selector.h selector.c \
	file.h file.c \
	encoding.h encoding.c \
	search.h search.c \
	dialog.h dialog.c \
	print.h print.c \
	about.h about.c \
	dnd.h dnd.c \
	utils.h utils.c \
	gtksourceiter.h gtksourceiter.c \
	i18n.h

leafpad_CFLAGS  = $(GTK_CFLAGS)
leafpad_LDADD   = $(GTK_LIBS) $(INTLLIBS)

