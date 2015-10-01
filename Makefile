#INCLUDES = -I/usr/lib/DXM/lib/Xm -I/usr/lib/DXM/lib/Xt -I/usr/lib/DXM/lib
#LDFLAGS = -L/usr/lib/DXM/lib/Xt -L/usr/lib/DXM/lib/Xm -L/usr/lib/DXM/lib

PROG=xmcolorscale

SRCS=\
    	main.c\
    	colorinfo.c\
	help.c \
	strdup.c \
	createWidgets.c

CC=cc
#CC=gcc

#OBJS= $(SRCS:.c=.o) /usr/lib/debug/malloc.o
OBJS= $(SRCS:.c=.o)

#CFLAGS = -c -g $(INCLUDES)
CFLAGS = -c -g -D_NO_PROTO $(INCLUDES)

#ifdef NEXT
LIBS=-lsys
#endif


LIBS =  -lXm -lXmu -lXt -lX11

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS) $(LIBS)

clean :
	rm *.o  $(PROG)

depend :
	sed '/^# DO NOT DELETE THIS LINE - used by/,$$d' Makefile > /tmp/$$
	mv /tmp/$$ Makefile
	echo "# DO NOT DELETE THIS LINE - used by 'make depend'" >>Makefile
	-(for i in ${SRCS} ; do \
	$(CC) $(CFLAGS) $(INCLUDES) -M $$i >> Makefile; \
	echo "	$(CC) $$(CFLAGS) $$i">>Makefile; done)

# DO NOT DELETE THIS LINE - used by 'make depend'
main.o : /NextDeveloper/Headers/X11/RectObj.h \
  /NextDeveloper/Headers/X11/X.h /NextDeveloper/Headers/X11/Xresource.h \
  /NextDeveloper/Headers/Xm/RowColumn.h \
  /NextDeveloper/Headers/X11/Object.h \
  /NextDeveloper/Headers/Xm/SelectioB.h \
  /NextDeveloper/Headers/ansi/stdio.h \
  /NextDeveloper/Headers/Xm/Protocols.h \
  /NextDeveloper/Headers/ansi/stdarg.h /NextDeveloper/Headers/X11/Xutil.h \
  /NextDeveloper/Headers/Xm/MainW.h /NextDeveloper/Headers/X11/Shell.h \
  /NextDeveloper/Headers/Xm/Xm.h /NextDeveloper/Headers/ansi/stddef.h \
  /NextDeveloper/Headers/X11/StringDefs.h \
  /NextDeveloper/Headers/X11/Xfuncproto.h \
  /NextDeveloper/Headers/Xm/FileSB.h /NextDeveloper/Headers/Xm/MessageB.h \
  /NextDeveloper/Headers/bsd/sys/types.h \
  /NextDeveloper/Headers/Xm/XmStrDefs.h \
  /NextDeveloper/Headers/X11/Composite.h \
  /NextDeveloper/Headers/architecture/ARCH_INCLUDE.h \
  /NextDeveloper/Headers/ansi/m68k/stddef.h \
  /NextDeveloper/Headers/ansi/m68k/stdarg.h \
  /NextDeveloper/Headers/X11/Constraint.h \
  /NextDeveloper/Headers/Xm/VirtKeys.h /NextDeveloper/Headers/Xm/Form.h \
  /NextDeveloper/Headers/X11/Intrinsic.h \
  /NextDeveloper/Headers/ansi/machine/stdarg.h \
  /NextDeveloper/Headers/bsd/machine/label_t.h \
  /NextDeveloper/Headers/Xm/BulletinB.h \
  /NextDeveloper/Headers/X11/Xosdefs.h \
  /NextDeveloper/Headers/ansi/string.h /NextDeveloper/Headers/X11/Core.h \
  /NextDeveloper/Headers/X11/Xlib.h \
  /NextDeveloper/Headers/bsd/m68k/label_t.h \
  /NextDeveloper/Headers/X11/cursorfont.h main.h \
  /NextDeveloper/Headers/Xm/VendorS.h /NextDeveloper/Headers/X11/Xatom.h \
  /NextDeveloper/Headers/ansi/machine/stddef.h \
  /NextDeveloper/Headers/Xm/PushB.h help.h \
  /NextDeveloper/Headers/Xm/CascadeB.h 
	cc $(CFLAGS) main.c
colorinfo.o : /NextDeveloper/Headers/X11/X.h \
  /NextDeveloper/Headers/ansi/stdio.h /NextDeveloper/Headers/ansi/stdarg.h \
  /NextDeveloper/Headers/ansi/stddef.h \
  /NextDeveloper/Headers/X11/Xfuncproto.h \
  /NextDeveloper/Headers/bsd/sys/types.h \
  /NextDeveloper/Headers/architecture/ARCH_INCLUDE.h \
  /NextDeveloper/Headers/ansi/m68k/stddef.h \
  /NextDeveloper/Headers/ansi/m68k/stdarg.h \
  /NextDeveloper/Headers/ansi/machine/stdarg.h \
  /NextDeveloper/Headers/bsd/machine/label_t.h \
  /NextDeveloper/Headers/X11/Xosdefs.h \
  /NextDeveloper/Headers/ansi/string.h /NextDeveloper/Headers/X11/Xlib.h \
  /NextDeveloper/Headers/bsd/m68k/label_t.h colorinfo.h \
  /NextDeveloper/Headers/ansi/machine/stddef.h 
	cc $(CFLAGS) colorinfo.c
help.o : /NextDeveloper/Headers/Xm/Text.h \
  /NextDeveloper/Headers/X11/RectObj.h /NextDeveloper/Headers/X11/X.h \
  /NextDeveloper/Headers/X11/Xresource.h \
  /NextDeveloper/Headers/X11/Object.h /NextDeveloper/Headers/ansi/stdio.h \
  /NextDeveloper/Headers/ansi/stdarg.h /NextDeveloper/Headers/X11/Xutil.h \
  /NextDeveloper/Headers/X11/Shell.h /NextDeveloper/Headers/Xm/Xm.h \
  /NextDeveloper/Headers/ansi/stddef.h \
  /NextDeveloper/Headers/X11/StringDefs.h \
  /NextDeveloper/Headers/X11/Xfuncproto.h \
  /NextDeveloper/Headers/Xm/MessageB.h \
  /NextDeveloper/Headers/bsd/sys/types.h \
  /NextDeveloper/Headers/Xm/XmStrDefs.h \
  /NextDeveloper/Headers/X11/Composite.h \
  /NextDeveloper/Headers/architecture/ARCH_INCLUDE.h \
  /NextDeveloper/Headers/ansi/m68k/stddef.h \
  /NextDeveloper/Headers/ansi/m68k/stdarg.h \
  /NextDeveloper/Headers/X11/Constraint.h \
  /NextDeveloper/Headers/Xm/Separator.h \
  /NextDeveloper/Headers/Xm/VirtKeys.h /NextDeveloper/Headers/Xm/Form.h \
  /NextDeveloper/Headers/X11/Intrinsic.h \
  /NextDeveloper/Headers/ansi/machine/stdarg.h \
  /NextDeveloper/Headers/bsd/machine/label_t.h \
  /NextDeveloper/Headers/Xm/BulletinB.h \
  /NextDeveloper/Headers/X11/Xosdefs.h \
  /NextDeveloper/Headers/ansi/string.h /NextDeveloper/Headers/X11/Core.h \
  /NextDeveloper/Headers/X11/Xlib.h \
  /NextDeveloper/Headers/bsd/m68k/label_t.h \
  /NextDeveloper/Headers/X11/cursorfont.h main.h \
  /NextDeveloper/Headers/Xm/VendorS.h /NextDeveloper/Headers/Xm/List.h \
  /NextDeveloper/Headers/X11/Xatom.h \
  /NextDeveloper/Headers/ansi/machine/stddef.h \
  /NextDeveloper/Headers/Xm/PushB.h 
	cc $(CFLAGS) help.c
createWidgets.o : /NextDeveloper/Headers/Xm/Text.h \
  /NextDeveloper/Headers/X11/RectObj.h /NextDeveloper/Headers/X11/X.h \
  /NextDeveloper/Headers/X11/Xresource.h \
  /NextDeveloper/Headers/Xm/RowColumn.h \
  /NextDeveloper/Headers/Xm/DrawingA.h /NextDeveloper/Headers/X11/Object.h \
  /NextDeveloper/Headers/Xm/SelectioB.h \
  /NextDeveloper/Headers/ansi/stdio.h \
  /NextDeveloper/Headers/Xm/SeparatoG.h \
  /NextDeveloper/Headers/ansi/stdarg.h /NextDeveloper/Headers/X11/Xutil.h \
  /NextDeveloper/Headers/X11/Shell.h /NextDeveloper/Headers/Xm/Xm.h \
  /NextDeveloper/Headers/ansi/stddef.h \
  /NextDeveloper/Headers/X11/StringDefs.h \
  /NextDeveloper/Headers/X11/Xfuncproto.h \
  /NextDeveloper/Headers/Xm/FileSB.h /NextDeveloper/Headers/Xm/MessageB.h \
  /NextDeveloper/Headers/bsd/sys/types.h /NextDeveloper/Headers/Xm/Scale.h \
  /NextDeveloper/Headers/Xm/XmStrDefs.h \
  /NextDeveloper/Headers/X11/Composite.h \
  /NextDeveloper/Headers/architecture/ARCH_INCLUDE.h \
  /NextDeveloper/Headers/ansi/m68k/stddef.h \
  /NextDeveloper/Headers/ansi/m68k/stdarg.h \
  /NextDeveloper/Headers/Xm/PanedW.h /NextDeveloper/Headers/X11/keysym.h \
  /NextDeveloper/Headers/X11/Constraint.h \
  /NextDeveloper/Headers/Xm/VirtKeys.h /NextDeveloper/Headers/Xm/Form.h \
  /NextDeveloper/Headers/ansi/machine/stdarg.h \
  /NextDeveloper/Headers/X11/Intrinsic.h \
  /NextDeveloper/Headers/bsd/machine/label_t.h \
  /NextDeveloper/Headers/Xm/BulletinB.h \
  /NextDeveloper/Headers/X11/Xosdefs.h \
  /NextDeveloper/Headers/ansi/string.h /NextDeveloper/Headers/X11/Core.h \
  /NextDeveloper/Headers/X11/Xlib.h \
  /NextDeveloper/Headers/bsd/m68k/label_t.h \
  /NextDeveloper/Headers/Xm/ToggleB.h \
  /NextDeveloper/Headers/X11/keysymdef.h colorinfo.h \
  /NextDeveloper/Headers/X11/cursorfont.h main.h \
  /NextDeveloper/Headers/Xm/VendorS.h /NextDeveloper/Headers/Xm/List.h \
  /NextDeveloper/Headers/Xm/TextF.h /NextDeveloper/Headers/X11/Xatom.h \
  /NextDeveloper/Headers/ansi/machine/stddef.h \
  /NextDeveloper/Headers/Xm/PushB.h help.h \
  /NextDeveloper/Headers/Xm/CascadeB.h 
	cc $(CFLAGS) createWidgets.c
