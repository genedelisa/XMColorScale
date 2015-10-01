#include <X11/Xlib.h>

#ifndef _COLORINFO_H
#define _COLORINFO_H

typedef struct _colorInfo {
     XColor xc;
     char *name;
     struct _colorInfo *next, *prev;
} COLORINFO, *COLORINFOPTR;

COLORINFOPTR getNthColor();
COLORINFOPTR insertColor();
COLORINFOPTR newColor();
COLORINFOPTR newColorInfo();
COLORINFOPTR insertColorInfo();


XColor *getColorFromList();

extern COLORINFOPTR colorList;
extern COLORINFOPTR currentColor;

#endif

