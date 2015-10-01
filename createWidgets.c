#include <stdio.h>

#include <string.h>
#include <X11/cursorfont.h>
#include <Xm/DrawingA.h>
#include <Xm/Scale.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/PanedW.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>

#include "main.h"
#include "colorinfo.h"
#include "help.h"

char *matchColor();
void setCurrentColorText();
Widget createList ();
void managePersonalList ();
void manageColormap ();
Window chooseWindow ();

Widget currentFgText, currentBgText;
Widget red, green, blue, gang, list;
Widget warningDialog;
Boolean setBack = True;
XColor colors[2];
Display * disp;
Colormap cmap;
Boolean gangBlue = True;
Boolean gangRed = True;
Boolean gangGreen = True;

extern int  badColorName;
extern  AppData appdata;




#if (XmVersion<1002)
#define XmStringCreateLocalized XmStringCreateSimple
#endif


void
popDown(w)
    Widget w;
{
    XtUnmanageChild(w);
}


XtActionProc
centerSliderAction (w, ev, args, nargs)
    Widget w;
    XEvent * ev;
    String args[];
    int     nargs;
{
    int     val,max;

    XtVaGetValues (XtParent(w),
		   XmNmaximum, &max,
		   NULL);

    XtVaSetValues (XtParent(w),
		   XmNuserData, (int) max / 2,
		   NULL);

    XmScaleSetValue(XtParent(w), (int)max / 2);
}

XtActionsRec actions[] = {
    {
	"centerSliderAction", (XtActionProc) centerSliderAction
    }
};


String scaleTranslations = "<Key> c : centerSliderAction()";


/*************************************************************************/
stealColor (w, cl, cd)
    Widget w;
    XtPointer cl;
    XtPointer cd;
{
    XColor * color;
    int     status;
    Pixel background;
    Pixel newpix;
    XmPushButtonCallbackStruct * cbs =
	(XmPushButtonCallbackStruct *) cd;

    Window chosen, ignorew, last;
    int     ignore;
    unsigned int    ignoreu;
    XEvent ev;
    int     x,
	y;
    XColor bgColor;
    char *name;

    status = XGrabPointer (disp,
			   XRootWindowOfScreen (XDefaultScreenOfDisplay (disp)),
			   FALSE,
			   ButtonPressMask,
			   GrabModeAsync,
			   GrabModeSync,
			   None,
			   XCreateFontCursor (disp, XC_crosshair),
			   CurrentTime);

    while (True) {
	XtAppNextEvent (XtWidgetToApplicationContext (w), &ev);
	if (ev.type == ButtonPress) {
	    break;
	}
	else {
	    XtDispatchEvent (&ev);
	}
    }

    if (ev.type != ButtonPress) {
	fprintf (stderr, "Impossible.\n");
	exit (1);
    }
    last = XDefaultRootWindow (disp);
    chosen = last;
    while (chosen != None) {
	XQueryPointer (disp, last, &ignorew, &chosen,
		       &ignore, &ignore, &ignore, &ignore, &ignoreu);
	if (chosen != None)
	    last = chosen;
    }
    XUngrabPointer (disp, CurrentTime);

    x = ((XButtonEvent *) & ev) -> x_root;
    y = ((XButtonEvent *) & ev) -> y_root;
    newpix = XGetPixel (XGetImage (disp,
				   XRootWindowOfScreen (XDefaultScreenOfDisplay (disp)),
				   x, y, 1, 1,
				   512 - 1, ZPixmap),
			0, 0);

    bgColor.pixel = newpix;
    XQueryColor (disp, cmap, &bgColor);

    if (setBack)
	color = &colors[0];
    else
	color = &colors[1];
    color -> red = bgColor.red;
    color -> green = bgColor.green;
    color -> blue = bgColor.blue;
    color -> flags = DoGreen | DoBlue | DoRed;
	if(appdata.forceNoncolor == False)
    	XStoreColor (disp, cmap, color);
    XmScaleSetValue (red, (int) color -> red / 256);
    XmScaleSetValue (green, (int) color -> green / 256);
    XmScaleSetValue (blue, (int) color -> blue / 256);

    XmListDeselectAllItems (list);
    updateEverybody(color);
	if((name = matchColor(color)) != NULL ){
                    XmString xms;
                    xms = XmStringCreateLocalized(name);
                    XmListSelectItem(list, xms, True);
                    XmListSetItem(list, xms);
	}


    XSync (disp, 0);
}


setOtherWindow (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    XColor color;
    int     jump = 1;
    Window win, target_win = None;
    Window root, parent, *kids;
    unsigned int    nKids,
                    dummyi;
    int     dummy;
    int     status;
    int     j;
    XWindowAttributes wind_info;
    XExposeEvent ev;

    target_win = chooseWindow (w);
    XGetWindowAttributes (disp, target_win, &wind_info);

    /* get it's own colorcell */
    /* if you use colors[0].pixel it will change when the scales are moved
       */
    color.pixel = colors[0].pixel;
    /* fill in the rgb values */
    XQueryColor (disp, cmap, &color);
    /* put it in the colormap and set the pixel (index) */
    XAllocColor (disp, wind_info.colormap, &color);

    XSetWindowBackground (disp,
	    target_win, color.pixel);
    XClearWindow (disp, target_win);

    ev.type = Expose;
    ev.display = disp;
    ev.window = target_win;
    ev.x = 0;
    ev.y = 0;
    ev.width = wind_info.width;
    ev.height = wind_info.height;
    ev.send_event = True;
    status = XSendEvent (disp, target_win, False, 0, (XEvent *) & ev);
    XSync (disp, 0);
}


toggleCB (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    int     which = (int) cl;

    if (which == 0)
	setBack = True;
    else
	setBack = False;
}

listCB (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    XColor * color;
    XmListCallbackStruct * cbs = (XmListCallbackStruct *) cd;
    char   *string;
    Widget theText;
    char buf[32];

    if (setBack)
	color = &colors[0];
    else
	color = &colors[1];

    XmStringGetLtoR (cbs -> item, XmSTRING_DEFAULT_CHARSET, &string);
	if(appdata.forceNoncolor == False)
    XStoreNamedColor (disp, cmap, string, color -> pixel, color -> flags);

    if (badColorName) {
	badColorName = 0;
    }

    XtFree (string);
    XQueryColor (disp, cmap, color);

    XmScaleSetValue (red, (int) color -> red / 256);
    XmScaleSetValue (green, (int) color -> green / 256);
    XmScaleSetValue (blue, (int) color -> blue / 256);

updateEverybody(color);

}
void
gangCB (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    int     r,
            g,
            b;
    int     lastValue;
    int     delta,
            redval,
            blueval,
            greenval;
    XmScaleCallbackStruct * cbs = (XmScaleCallbackStruct *) cd;
    XColor * color;
    Widget theText;
    char buf[32];

    if (setBack)
	color = &colors[0];
    else
	color = &colors[1];

    XtVaGetValues (w,
	    XmNuserData, &lastValue,
	    NULL);
/*printf("cb %x\n", w);*/

    delta = (cbs -> value - lastValue);

    XtVaSetValues (w,
	    XmNuserData, (int) cbs -> value,
	    NULL);

    XmScaleGetValue (red, &r);
    XmScaleGetValue (green, &g);
    XmScaleGetValue (blue, &b);

    redval = (r + delta);
    greenval = (g + delta);
    blueval = (b + delta);

    if (redval < 0 || redval > 255) {
	XBell (XtDisplay (w), 100);
	redval = redval < 0 ? 255 : 0;
    }
    if (blueval < 0 || blueval > 255) {
	XBell (XtDisplay (w), 100);
	blueval = blueval < 0 ? 255 : 0;
    }
    if (greenval < 0 || greenval > 255) {
	XBell (XtDisplay (w), 100);
	greenval = greenval < 0 ? 255 : 0;
    }

    if (gangBlue) {
	XmScaleSetValue (blue, blueval);
	color -> blue = (65536 * (long) blueval) / 256;
    }
    if (gangRed) {
	XmScaleSetValue (red, redval);
	color -> red = (65536 * (long) redval) / 256;
    }
    if (gangGreen) {
	XmScaleSetValue (green, greenval);
	color -> green = (65536 * (long) greenval) / 256;
    }

    color -> flags = DoGreen | DoBlue | DoRed;
	if(appdata.forceNoncolor == False)
    XStoreColor (disp, cmap, color);
    XmListDeselectAllItems (list);

    XtVaGetValues (red,
	    XmNuserData, &theText,
	    NULL);
    sprintf(buf, "%d",  redval);
    XmTextFieldSetString(theText, buf);

    XtVaGetValues (green,
	    XmNuserData, &theText,
	    NULL);
    sprintf(buf, "%d", greenval);
    XmTextFieldSetString(theText, buf);

    XtVaGetValues (blue,
	    XmNuserData, &theText,
	    NULL);
    sprintf(buf, "%d", blueval);
    XmTextFieldSetString(theText, buf);

    if (setBack)
	setCurrentColorText(currentBgText, color);
    else
	setCurrentColorText(currentFgText, color);

}
doGreen (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    XColor * color;
    XmScaleCallbackStruct * cbs = (XmScaleCallbackStruct *) cd;
    Widget text = (Widget) cl;
    char    buf[128];
    long value = cbs->value + 1;
    char *name;

    if (setBack)
	color = &colors[0];
    else
	color = &colors[1];

    color -> green = (65535 * value) / 256;
    color -> flags = DoGreen;
	if(appdata.forceNoncolor == False)
    XStoreColor (disp, cmap, color);
    XmListDeselectAllItems (list);
    sprintf (buf, "%d", cbs -> value);
    XmTextFieldSetString (text, buf);
    if (setBack)
	setCurrentColorText(currentBgText, color);
    else
	setCurrentColorText(currentFgText, color);
	
	if((name = matchColor(color)) != NULL ){
                    XmString xms;
                    xms = XmStringCreateLocalized(name);
                    XmListSelectItem(list, xms, True);
                    XmListSetItem(list, xms);
	}

}
doBlue (w, cl, cd)
    Widget w;
    XtPointer cl;
    XtPointer cd;
{
    XColor * color;
    XmScaleCallbackStruct * cbs = (XmScaleCallbackStruct *) cd;
    Widget text = (Widget) cl;
    char    buf[128];
    long value = cbs->value + 1;
    char *name;

    if (setBack)
	color = &colors[0];
    else
	color = &colors[1];

    color -> blue = (65535 * value) / 256;
    color -> flags = DoBlue;
	if(appdata.forceNoncolor == False)
    XStoreColor (disp, cmap, color);
    XmListDeselectAllItems (list);
    sprintf (buf, "%d", cbs -> value);
    XmTextFieldSetString (text, buf);
    if (setBack)
	setCurrentColorText(currentBgText, color);
    else
	setCurrentColorText(currentFgText, color);

	if((name = matchColor(color)) != NULL ){
                    XmString xms;
                    xms = XmStringCreateLocalized(name);
                    XmListSelectItem(list, xms, True);
                    XmListSetItem(list, xms);
	}

}
doRed (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
     XColor * color;
     XmScaleCallbackStruct * cbs = (XmScaleCallbackStruct *) cd;
     Widget text = (Widget) cl;
     char    buf[128];
     long value = cbs->value + 1;
    char *name;

     if (setBack)
	 color = &colors[0];

     else
	  color = &colors[1];

     color -> red = (65535 * value) / 256;
     color -> flags = DoRed;
	if(appdata.forceNoncolor == False)
     XStoreColor (disp, cmap, color);
     XmListDeselectAllItems (list);
     sprintf (buf, "%d",  cbs->value);
     XmTextFieldSetString (text, buf);
     if (setBack)
	 setCurrentColorText(currentBgText, color);
     else
	 setCurrentColorText(currentFgText, color);

	if((name = matchColor(color)) != NULL ){
                    XmString xms;
                    xms = XmStringCreateLocalized(name);
                    XmListSelectItem(list, xms, True);
                    XmListSetItem(list, xms);
	}

}
#include <X11/keysym.h>
#ifdef _NO_PROTO
listKeys(w, cl, ev, flag)
     Widget w;
     XtPointer cl;
     XEvent *ev;
     Boolean *flag;
#else
listKeys( Widget w, XtPointer cl,
     XEvent *ev, Boolean *flag)
#endif
{
     KeySym sym;
     XComposeStatus compose;
     char buffer[128];
     static char buffer2[255];
     int nbytes = 128, len;
     XmString *table;
     int icount, j;
     char *text;
     static int counter=0;

     if(ev->type == LeaveNotify) {
	  counter = 0;
	  return;
     }

     if(ev->type == KeyPress) {
	  len = XLookupString( (XKeyEvent *)ev, buffer, nbytes, &sym,
			       &compose);
	  if(len == 0) {
	       return;
	  }
	  if(sym == XK_Down) {
	       counter = 0;
	       return;
	  }
	  if(sym == XK_Return) {
	       counter = 0;
	       return;
	  }
	  if(sym == XK_Up) {
	       counter = 0;
	       return;
	  }
	  if(sym == XK_Escape) {
	       counter = 0;
	       return;
	  }
	  if(counter == 255) {
	       printf("counter at max\n");
	       return;
	  }
	
	  buffer2[counter++] = buffer[0];
	  buffer2[counter] = 0;
	
	  XtVaGetValues(w,
			XmNitems,	  &table,
			XmNitemCount,	  &icount,
			NULL);
	  for(j=0; j< icount; j++) {
	       if( !XmStringGetLtoR(table[j], XmFONTLIST_DEFAULT_TAG, &text))
		    break;
/*	       printf("<%s> <%s>\n", buffer2, text);*/
	       if( strncmp(buffer2, text, counter) == 0) {
		    XmListSelectItem(w, table[j], True);
		    XmListSetItem(w, table[j]);
		    XtFree(text);
		    return;
	       }
	       XtFree(text);
	  }
	  XBell(XtDisplay(w), 100);
	  printf("color %s is not in the list\n", buffer2);
	  counter = 0;
     }
}
Widget
createList (parent, top)
Widget parent, top;
{
     Widget list;
     int count, icount;

     list = XmCreateScrolledList (parent, "list", NULL, 0);
     XtVaSetValues (XtParent (list),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, top,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    NULL);

     XtAddCallback (list, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

     fillList (list, appdata.rgbFile);
     XtVaGetValues (list,
		    XmNvisibleItemCount, &count,
		    XmNitemCount, &icount,
		    NULL);

     XtVaSetValues (list,
		    XmNselectionPolicy, XmSINGLE_SELECT,
		    XmNsliderSize, icount / count,
		    NULL);

     XtAddEventHandler(list, KeyPressMask| LeaveWindowMask ,
		       False, listKeys, NULL);

     return (list);
}

fillList (list, filename)
Widget list;
char   *filename;
{
    char    buffer[255];
    FILE * fp;
    char    color[255];
    XmString xms;
    int     r,b,g;

    if ((fp = fopen (filename, "r")) == NULL) {
	sprintf (buffer, " opening the rgb file: %s ", filename);
	perror (buffer);
	exit (-1);
    }
    while (!feof (fp)) {
	char   *ptr;
        XColor xc;
	if ((ptr = fgets (buffer, 255, fp)) == NULL)
	    break;
	if (sscanf (buffer, "%d %d %d %[\tA-Za-z0-9 ]", &r, &g, &b, color)
		< 4) {
	    fprintf (stderr, "invalid rgb file specified");
	    return;
	}


	/* dont want the duplicates with spaces */
	if ((ptr = strchr (color, ' ')) == NULL) {
	    if (strstr (color, "grey") == NULL) {
		xms = XmStringCreateSimple (color);
		XmListAddItemUnselected (list, xms, 0);
		XmStringFree (xms);
	        xc.red = r;
	        xc.green = g;
	        xc.blue = b;
	        addToColorList(&xc, color);
	    }
	}
    }
    fclose (fp);

    /*printList(stdout, colorList);*/
}
fillPersonalList (list, filename)
Widget list;
char   *filename;
{
    char    buffer[255];
    FILE * fp;
    char    colorName[255];
    XmString xms;
    int     r,
            g,
            b;
    XColor color;

    if (filename == NULL)
	return;

    if ((fp = fopen (filename, "r")) == NULL) {
	sprintf (buffer, " opening the rgb file: %s ", filename);
	perror (buffer);
	exit (-1);
    }
    while (!feof (fp)) {
	char   *ptr;
	if ((ptr = fgets (buffer, 255, fp)) == NULL)
	    break;
	if (sscanf (buffer, "%d %d %d %[\tA-Za-z0-9 ]", &r, &g, &b, colorName)
		< 4) {
	    fprintf (stderr, "invalid rgb file specified");
	    return;;
	}

	/* dont want the duplicates with spaces */
	if ((ptr = strchr (colorName, ' ')) == NULL) {
	    /* dont want the duplicates with british spelling either */
	    if (strstr (colorName, "grey") == NULL) {
		xms = XmStringCreateSimple (colorName);
		XmListAddItemUnselected (list, xms, 0);
		XmStringFree (xms);

		color.red = r;
		color.green = g;
		color.blue = b;
		addToColorList (&color, colorName);
	    }
	}
    }
    fclose (fp);
}

gangToggleCB (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    String str = (String) cl;
    XmToggleButtonCallbackStruct * cbs =
	(XmToggleButtonCallbackStruct *) cd;

    if (strcmp (str, "red") == 0) {
	if (cbs -> set == True)
	    gangRed = True;
	else
	    gangRed = False;
    }
    if (strcmp (str, "blue") == 0) {
	if (cbs -> set == True)
	    gangBlue = True;
	else
	    gangBlue = False;
    }
    if (strcmp (str, "green") == 0) {
	if (cbs -> set == True)
	    gangGreen = True;
	else
	    gangGreen = False;
    }
}

#ifdef _NO_PROTO
scaleTextCB (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
#else
scaleTextCB (
Widget w,
XtPointer cl,
XtPointer cd)
#endif
{
    Widget scale = (Widget) cl;
    int     val;
    long    l;
    String str;
    XmScaleCallbackStruct cbs;

    str = XmTextFieldGetString (w);
    if (strncmp (str, "0x", 2) == 0) {
	l = strtol (str, NULL, 16);
	val = (int) l;
    }
    else
	val = atoi (str);
    XtFree (str);
    XmScaleSetValue (scale, val);
    cbs.value = val;
    cbs.reason = XmCR_VALUE_CHANGED;
    XtCallCallbacks(scale, XmNvalueChangedCallback, &cbs);

}

createWidgets (parent, app)
Widget parent;
XtAppContext app;
{
    extern  Widget menuBar;
    Widget w;
    Widget rc, text, radio;
    Widget currentrc;
    Widget tb1, tb2, tb3;
    Widget redtb, greentb, bluetb;
    Widget redtext, greentext, bluetext;
    Widget redForm, greenForm, blueForm;
    XmString xms;
    int     j;
    Dimension width, height;

    XtAddCallback (parent, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    createOptionMenu (menuBar);
    createHelpDialog(parent);
    warningDialog = XmCreateWarningDialog (parent, "message", NULL, 0);
    w = XmMessageBoxGetChild (warningDialog, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild (w);
    w = XmMessageBoxGetChild (warningDialog, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild (w);

    /* text = XtVaCreateManagedWidget ("text", xmTextWidgetClass, */

    text = XmCreateScrolledText (parent, "text", NULL, 0);
    XtVaSetValues (text,
		   XmNbackground, colors[0].pixel,
		   XmNforeground, colors[1].pixel,
		   XmNrows,	  10,
		   XmNcolumns,	  80,
		   XmNeditMode,	  XmMULTI_LINE_EDIT,
		   XmNeditable,	  True,
		   NULL);
    XtVaSetValues ( XtParent(text),
		    XmNtopAttachment,	XmATTACH_WIDGET,
		    XmNtopWidget,	menuBar,
		    XmNleftAttachment,	XmATTACH_FORM,
		    XmNrightAttachment,	XmATTACH_FORM,
		    NULL);
    XtManageChild (text);
    XtAddCallback (text, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);


    rc = XtVaCreateWidget ("rc", xmRowColumnWidgetClass, parent,
			   XmNorientation,     XmVERTICAL,
			   XmNtopAttachment,   XmATTACH_WIDGET,
			   XmNtopWidget,       XtParent(text),
			   XmNleftAttachment,  XmATTACH_FORM,
			   XmNrightAttachment, XmATTACH_FORM,
			   NULL);
    XtAddCallback (rc, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);


    redForm = XtVaCreateWidget ("redForm", xmFormWidgetClass, rc,
				NULL);
    xms = XmStringCreateSimple ("Red");
    red = XtVaCreateManagedWidget ("red", xmScaleWidgetClass, redForm,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNorientation,    XmHORIZONTAL,
				   XmNshowValue,      True,
				   XmNmaximum,	      255,
				   XmNtitleString,    xms,
				   NULL);
    XmStringFree (xms);
    XtVaGetValues (red,
		   XmNheight, &height,
		   NULL);
    XtAddCallback (red, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);


    redtext = XtVaCreateManagedWidget ("redtext", xmTextFieldWidgetClass,
				       redForm,
				       XmNleftAttachment,   XmATTACH_WIDGET,
				       XmNleftWidget,	    red,
				       XmNtopAttachment,    XmATTACH_FORM,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNtopOffset,	    height / 4,
				       XmNbottomOffset,	    height / 4,
				       XmNcolumns,	    4,
				       NULL);
    XtAddCallback (redtext, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);
    XmTextFieldSetString(redtext, "0");
    XtVaSetValues(red,
		  XmNuserData, redtext,
		  NULL);


    redtb = XtVaCreateManagedWidget ("redtb", xmToggleButtonWidgetClass,
				     redForm,
				     XmNleftAttachment,	  XmATTACH_WIDGET,
				     XmNleftWidget,	  redtext,
				     XmNtopAttachment,	  XmATTACH_FORM,
				     XmNbottomAttachment, XmATTACH_FORM,
				     XmNtopOffset,	  height / 4,
				     XmNbottomOffset,	  height / 4,
				     XmNset,		  True,
				     NULL);
    XtAddCallback (redtb, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    XtManageChild (redForm);


    greenForm = XtVaCreateWidget ("greenForm", xmFormWidgetClass, rc,
				  NULL);
    xms = XmStringCreateSimple ("Green");
    green = XtVaCreateManagedWidget ("green", xmScaleWidgetClass, greenForm,
				     XmNleftAttachment,	XmATTACH_FORM,
				     XmNorientation,	XmHORIZONTAL,
				     XmNshowValue,	True,
				     XmNmaximum,	255,
				     XmNtitleString,	xms,
				     NULL);
    XmStringFree (xms);
    XtVaGetValues (green,
		   XmNheight, &height,
		   NULL);
    XtAddCallback (green, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    greentext = XtVaCreateManagedWidget ("greentext", xmTextFieldWidgetClass,
					 greenForm,
					 XmNleftAttachment,   XmATTACH_WIDGET,
					 XmNleftWidget,	      green,
					 XmNtopAttachment,    XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNtopOffset,	      height / 4,
					 XmNbottomOffset,     height / 4,
					 XmNcolumns,	      4,
					 NULL);
    XtAddCallback (greentext, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);
    XmTextFieldSetString(greentext, "0");
    XtVaSetValues(green,
		  XmNuserData, greentext,
		  NULL);

    greentb = XtVaCreateManagedWidget ("greentb", xmToggleButtonWidgetClass,
				       greenForm,
				       XmNleftAttachment,   XmATTACH_WIDGET,
				       XmNleftWidget,	    greentext,
				       XmNtopAttachment,    XmATTACH_FORM,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNtopOffset,	    height / 4,
				       XmNbottomOffset,	    height / 4,
				       XmNset,		    True,
				       NULL);
    XtAddCallback (greentb, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    XtManageChild (greenForm);



    blueForm = XtVaCreateWidget ("blueForm", xmFormWidgetClass, rc,
				 NULL);

    xms = XmStringCreateSimple ("Blue");
    blue = XtVaCreateManagedWidget ("blue", xmScaleWidgetClass, blueForm,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNorientation,    XmHORIZONTAL,
				    XmNshowValue,      True,
				    XmNmaximum,	       255,
				    XmNtitleString,    xms,
				    NULL);
    XmStringFree (xms);
    XtVaGetValues (green,
		   XmNheight, &height,
		   NULL);
    XtAddCallback (blue, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    bluetext = XtVaCreateManagedWidget ("bluetext", xmTextFieldWidgetClass,
					blueForm,
					XmNleftAttachment,   XmATTACH_WIDGET,
					XmNleftWidget,	     blue,
					XmNtopAttachment,    XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNtopOffset,	     height / 4,
					XmNbottomOffset,     height / 4,
					XmNcolumns,	     4,
					NULL);
    XtAddCallback (bluetext, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);
    XmTextFieldSetString(bluetext, "0");
    XtVaSetValues(blue,
		  XmNuserData, bluetext,
		  NULL);

    bluetb = XtVaCreateManagedWidget ("bluetb", xmToggleButtonWidgetClass,
				      blueForm,
				      XmNleftAttachment,   XmATTACH_WIDGET,
				      XmNleftWidget,	   bluetext,
				      XmNtopAttachment,	   XmATTACH_FORM,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNtopOffset,	   height / 4,
				      XmNbottomOffset,	   height / 4,
				      XmNset,		   True,
				      NULL);
    XtManageChild (blueForm);
    XtAddCallback (bluetb, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    XtAddCallback (redtext, XmNactivateCallback, scaleTextCB, red);
    XtAddCallback (greentext, XmNactivateCallback, scaleTextCB, green);
    XtAddCallback (bluetext, XmNactivateCallback, scaleTextCB, blue);

    XtAddCallback (redtb, XmNvalueChangedCallback, gangToggleCB, "red");
    XtAddCallback (greentb, XmNvalueChangedCallback, gangToggleCB, "green");
    XtAddCallback (bluetb, XmNvalueChangedCallback, gangToggleCB, "blue");

    xms = XmStringCreateSimple ("Gang");
    gang = XtVaCreateManagedWidget ("gang", xmScaleWidgetClass, rc,
				    XmNorientation, XmHORIZONTAL,
				    XmNshowValue,   False,
				    XmNmaximum,	    255,
				    XmNvalue,	    127,
				    XmNuserData,    127,
				    XmNtitleString, xms,
				    NULL);
    XmStringFree (xms);
    XtAddCallback (gang, XmNvalueChangedCallback, gangCB, NULL);
    XtAddCallback (gang, XmNdragCallback, gangCB, NULL);
    XtOverrideTranslations (gang,
			    XtParseTranslationTable (scaleTranslations));
    for (j = 0; j <= 25; j++)
	XtVaCreateManagedWidget ("tick", xmSeparatorGadgetClass, gang,
				 XmNorientation, XmVERTICAL,
				 NULL);
    XtAddCallback (gang, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    radio = XmCreateRadioBox (parent, "radio", NULL, 0);
    XtVaSetValues (radio,
		   XmNorientation,     XmHORIZONTAL,
		   XmNtopAttachment,   XmATTACH_WIDGET,
		   XmNtopWidget,       rc,
		   XmNleftAttachment,  XmATTACH_FORM,
		   XmNrightAttachment, XmATTACH_FORM,
		   NULL);
    XtAddCallback (radio, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    tb1 = XtVaCreateManagedWidget ("tb1", xmToggleButtonWidgetClass, radio,
				   NULL);
    tb2 = XtVaCreateManagedWidget ("tb2", xmToggleButtonWidgetClass, radio,
				   NULL);
    XtAddCallback (tb1, XmNvalueChangedCallback, toggleCB, (XtPointer) 0);
    XtAddCallback (tb2, XmNvalueChangedCallback, toggleCB, (XtPointer) 1);
    XtAddCallback (tb1, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);
    XtAddCallback (tb2, XmNhelpCallback, helpCB,
		   (XtPointer) NULL);

    currentrc = XtVaCreateWidget ("currentrc", xmRowColumnWidgetClass, parent,
			   XmNorientation,     XmHORIZONTAL,
			   XmNtopAttachment,   XmATTACH_WIDGET,
			   XmNtopWidget,       radio,
			   XmNleftAttachment,  XmATTACH_FORM,
			   XmNrightAttachment, XmATTACH_FORM,
			   NULL);
    currentBgText = XtVaCreateManagedWidget("curBgText",
					    xmTextFieldWidgetClass,
					    currentrc,
					    XmNeditable, False,
					    XmNcolumns, 20,
					    NULL);
    currentFgText = XtVaCreateManagedWidget("curFgText",
					    xmTextFieldWidgetClass,
					    currentrc,
					    XmNeditable, False,
					    XmNcolumns, 20,
					    NULL);
    XmTextFieldSetString(currentBgText, "black");
    XmTextFieldSetString(currentFgText, "white");

    list = createList (parent, currentrc);

   XtAddCallback (list, XmNdefaultActionCallback, listCB,
		   (XtPointer) NULL);
    XtAddCallback (list, XmNsingleSelectionCallback, listCB,
		   (XtPointer) NULL);

    XtManageChild (radio);
    XtManageChild (rc);
    XtManageChild (currentrc);
    XtManageChild (list);

    XtAddCallback (red, XmNvalueChangedCallback, doRed,
		   (XtPointer) redtext);

    XtAddCallback (red, XmNhelpCallback, helpCB,
		   (XtPointer) 2);

    XtAddCallback (green, XmNvalueChangedCallback, doGreen,
		   (XtPointer) greentext);
    XtAddCallback (blue, XmNvalueChangedCallback, doBlue,
		   (XtPointer) bluetext);

    XtAddCallback (red, XmNdragCallback, doRed,
		   (XtPointer) redtext);
    XtAddCallback (green, XmNdragCallback, doGreen,
		   (XtPointer) greentext);
    XtAddCallback (blue, XmNdragCallback, doBlue,
		   (XtPointer) bluetext);

    XtAppAddActions (XtWidgetToApplicationContext (parent),
		     actions, XtNumber (actions));

}
void
setupColors (parent)
Widget parent;
{
    int     screen;
    Pixel thePixels[2];


    disp = XtDisplay (parent);
    screen = DefaultScreen (disp);
    cmap = DefaultColormap (disp, screen);

    /* alloc r/w cells. ignores planes */
    XAllocColorCells (disp, cmap, False, NULL, 0, thePixels, 2);

    /* now write to the color cells */
    colors[0].pixel = thePixels[0];
    colors[0].flags = DoRed | DoGreen | DoBlue;
	if(appdata.forceNoncolor == False)
    XStoreNamedColor (disp, cmap, "Black", colors[0].pixel, colors[0].flags);

    colors[1].pixel = thePixels[1];
    colors[1].flags = DoRed | DoGreen | DoBlue;
	if(appdata.forceNoncolor == False)
    XStoreNamedColor (disp, cmap, "White", colors[1].pixel, colors[1].flags);
}
void
saveList (list, filename)
Widget list;
char   *filename;
{
    FILE * fp;
    char    buffer[255];

    if ((fp = fopen (filename, "w")) == NULL) {
	sprintf (buffer, " opening the rgb file: %s ", filename);
	perror (buffer);
	return;
    }

    printColors (fp);
    fclose (fp);
}

createOptionMenu (parent)
Widget parent;
{
    Widget cascade, otherWinPB, personalPB, colormapPB, stealPB;
    Widget optionPulldown;


    optionPulldown = XmCreatePulldownMenu (parent, "mainWinOptionPulldown",
	    NULL, 0);

    otherWinPB = XtVaCreateManagedWidget ("otherWinPB", xmPushButtonWidgetClass,
	    optionPulldown,
	    NULL);
    XtAddCallback (otherWinPB, XmNactivateCallback, setOtherWindow,
	    (XtPointer) NULL);

    personalPB = XtVaCreateManagedWidget ("personalPB", xmPushButtonWidgetClass,
	    optionPulldown,
	    NULL);
    XtAddCallback (personalPB, XmNactivateCallback, managePersonalList,
	    (XtPointer) NULL);

    colormapPB = XtVaCreateManagedWidget ("colormapPB",
	    xmPushButtonWidgetClass,
	    optionPulldown,
	    NULL);
    XtAddCallback (colormapPB, XmNactivateCallback, manageColormap,
	    (XtPointer) NULL);

    stealPB = XtVaCreateManagedWidget ("stealPB",
	    xmPushButtonWidgetClass,
	    optionPulldown,
	    NULL);
    XtAddCallback (stealPB, XmNactivateCallback, stealColor,
	    (XtPointer) NULL);

    cascade = XtVaCreateManagedWidget ("mainWinOptionCascade",
	    xmCascadeButtonWidgetClass, parent,
	    XmNsubMenuId, optionPulldown,
	    NULL);

#if (XmVersion>=1002)
    XtVaSetValues (optionPulldown,
	    XmNtearOffModel, XmTEAR_OFF_ENABLED,
	    NULL);
#endif
}


deleteFromList (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    Widget sel = (Widget) cl;
    Widget list;
    String str;
    XmString xmstr;
    XmStringTable items;
    int     selectedItemCount;
    int     pos;

    list = XmSelectionBoxGetChild (sel, XmDIALOG_LIST);
    XtVaGetValues (list,
	    XmNselectedItems, &items,
	    XmNselectedItemCount, &selectedItemCount,
	    NULL);
    if (selectedItemCount == 0)
	return;


    pos = XmListItemPos (list, items[0]);
    pos--;
    deleteFromColorList (pos);

    XmListDeleteItem (list, items[0]);

}
personalListCB (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    XColor * color;
    XColor * listColor;
    XmSelectionBoxCallbackStruct * cbs =
	(XmSelectionBoxCallbackStruct *) cd;
    Widget list = (Widget) cl;
    int     pos;

    if (setBack)
	color = &colors[0];
    else
	color = &colors[1];

    /* find the color in the list */
    pos = XmListItemPos (list, cbs -> value);
    pos--;

    listColor = getColorFromList (pos);
    if (listColor == NULL) {
	printf ("cannot find color\n");
	return;
    }


    color -> red = (65536 * (long) listColor -> red) / 256;
    color -> green = (65536 * (long) listColor -> green) / 256;
    color -> blue = (65536 * (long) listColor -> blue) / 256;
    color -> flags = DoGreen | DoBlue | DoRed;

	if(appdata.forceNoncolor == False)
    XStoreColor (disp, cmap, color);

    XmScaleSetValue (red, (int) listColor -> red);
    XmScaleSetValue (green, (int) listColor -> green);
    XmScaleSetValue (blue, (int) listColor -> blue);
	updateEverybody(color);
}
addToList (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    Widget sel = (Widget) cl;
    Widget text, list;
    String str = NULL;
    XmString xmstr;
    XColor color;
    int     r,
            g,
            b;

    text = XmSelectionBoxGetChild (sel, XmDIALOG_TEXT);
    list = XmSelectionBoxGetChild (sel, XmDIALOG_LIST);

    str = XmTextGetString (text);
    if (strlen (str) == 0)
	return;
    xmstr = XmStringCreateSimple (str);
    XmListAddItemUnselected (list, xmstr, 0);
    XmStringFree (xmstr);

/* get the current scale values */
    XmScaleGetValue (red, &r);
    XmScaleGetValue (green, &g);
    XmScaleGetValue (blue, &b);
    color.red = r;
    color.green = g;
    color.blue = b;
    addToColorList (&color, str);

    XtFree (str);
}

void
readPersonalFile (w, cl, cd)
Widget w;
XtPointer cl, cd;
{
    XmFileSelectionBoxCallbackStruct * cb =
	(XmFileSelectionBoxCallbackStruct *) cd;
    char   *filename;
    Widget list = (Widget) cl;

    if (!XmStringGetLtoR (cb -> value, XmSTRING_DEFAULT_CHARSET, &filename)) {
	return;
    }

    fillPersonalList (list, filename);
    XtFree (filename);
}
openPersonalFile (w, cl, cd)
Widget w;
XtPointer cl, cd;
{
    static  Widget fsd;
    Widget list = (Widget) cl;

    if (!fsd) {
	fsd = XmCreateFileSelectionDialog (w, "openPersonalDialog", NULL, 0);
	XtAddCallback (fsd, XmNokCallback, readPersonalFile, (XtPointer) list);
	XtAddCallback (fsd, XmNcancelCallback, popDown,
		(XtPointer) NULL);
    }
    XtManageChild (fsd);
}


void
managePersonalList (w, cl, cd)
    Widget w;
    XtPointer cl;
    XtPointer cd;
{
    Widget readPB, add, delete, name, list;
    XColor rgb;
    static  Widget selectionDialog;

    if (!selectionDialog) {
	Widget form;
	selectionDialog =
	    XmCreateSelectionDialog (w, "selectionDialog", NULL, 0);
	XtVaSetValues (selectionDialog,
		       XmNmustMatch, True,
		       NULL);

	list = XmSelectionBoxGetChild (selectionDialog, XmDIALOG_LIST);
	fillPersonalList (list, appdata.personalRgbFile);

	XtAddCallback (list, XmNsingleSelectionCallback, personalListCB,
		       (XtPointer) list);
	XtAddCallback (selectionDialog, XmNokCallback, personalListCB,
		       (XtPointer) list);
	XtAddCallback (selectionDialog, XmNapplyCallback, personalListCB,
		       (XtPointer) list);

	/* form = XtVaCreateWidget ("form", xmFormWidgetClass, */
	form = XtVaCreateWidget ("form", xmRowColumnWidgetClass,
				 selectionDialog,
				 XmNpacking,	 XmPACK_COLUMN,
				 XmNorientation, XmHORIZONTAL,
				 NULL);

	add = XtVaCreateManagedWidget ("addPB", xmPushButtonWidgetClass,
				       form,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNtopAttachment,    XmATTACH_FORM,
				       XmNleftAttachment,   XmATTACH_POSITION,
				       XmNleftPosition,	    1,
				       XmNrightAttachment,  XmATTACH_NONE,
				       NULL);
	delete = XtVaCreateManagedWidget ("deletePB", xmPushButtonWidgetClass,
					  form,
					  XmNtopAttachment,    XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNleftAttachment,   XmATTACH_POSITION,
					  XmNleftPosition,     33,
					  XmNrightAttachment,  XmATTACH_NONE,
					  NULL);
	readPB = XtVaCreateManagedWidget ("readPB", xmPushButtonWidgetClass,
					  form,
					  XmNtopAttachment,    XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNleftAttachment,   XmATTACH_POSITION,
					  XmNleftPosition,     66,
					  XmNrightAttachment,  XmATTACH_NONE,
					  NULL);
	XtManageChild (form);

	XtAddCallback (readPB, XmNactivateCallback, openPersonalFile,
		       (XtPointer) list);

	XtAddCallback (add, XmNactivateCallback, addToList,
		       (XtPointer) selectionDialog);
	XtAddCallback (delete, XmNactivateCallback, deleteFromList,
		       (XtPointer) selectionDialog);

	XtAddCallback (selectionDialog, XmNcancelCallback, popDown,
		       (XtPointer) selectionDialog);
    }

    XtManageChild (selectionDialog);
}

void
showrgbvals (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    Widget text = (Widget) cl;
    XColor color;
    Pixel thePix;
    char    buffer[128];

    XtVaGetValues (w,
	    XmNuserData, &thePix,
	    NULL);

    color.pixel = thePix;
    XQueryColor (disp, cmap, &color);
    sprintf (buffer, "%3.3d %3.3d %3.3d (%3.3d %3.3d %3.3d)",
	    color.red, color.green, color.blue,
	    color.red / 256, color.green / 256, color.blue / 256);
    XmTextFieldSetString (text, buffer);
}


/* we dont want the act of observing the colormap to change it */
void
heisenberg (bg,fg, select,top,bottom) 
	XColor *bg;
	XColor *fg;
	XColor *select;
	XColor *top;
	XColor *bottom;
{
	bg->red = bg->green = bg->blue = 0;
	fg->red = fg->green = fg->blue = 0;
	select->red = select->green = select->blue = 0;
	top->red = top->green = top->blue = 0;
	bottom->red = bottom->green = bottom->blue = 0;
}

void
manageColormap (w, cl, cd)
    Widget w;
    XtPointer cl;
    XtPointer cd;
{
    Widget rc, pb, r, b, g;
    XColor rgb;
    static  Widget colormapDialog;
    XmColorProc oldproc;
    static Cursor cursor;
    extern Widget mainWin;

    if(!cursor)
	cursor = XCreateFontCursor (XtDisplay(w), XC_watch);
    XDefineCursor(XtDisplay(w), XtWindow(mainWin), cursor);

    oldproc = XmSetColorCalculation (heisenberg);

    if (!colormapDialog) {
	int     x,y;
	char    buffer[128];

	colormapDialog =
	    XmCreateFormDialog (w, "colormapDialog", NULL, 0);
	rc = XtVaCreateWidget ("rc", xmRowColumnWidgetClass,
			       colormapDialog,
			       XmNtopAttachment, XmATTACH_FORM,
			       XmNnumColumns,	 16,
			       XmNorientation,	 XmVERTICAL,
			       XmNpacking,	 XmPACK_COLUMN,
			       NULL);
	r = XtVaCreateManagedWidget ("r", xmTextFieldWidgetClass,
				     colormapDialog,
				     XmNeditable,	  False,
				     XmNtopAttachment,	  XmATTACH_WIDGET,
				     XmNtopWidget,	  rc,
				     XmNrightAttachment,  XmATTACH_FORM,
				     XmNleftAttachment,	  XmATTACH_FORM,
				     NULL);
	for (x = 0; x <= 255; x++) {
	    sprintf (buffer, "%d", x);
	    pb = XtVaCreateManagedWidget (buffer,
					  xmPushButtonWidgetClass, rc,
					  XmNbackground, (Pixel) x,
					  XmNuserData,	 (Pixel) x,
					  NULL);
	    XtAddCallback (pb, XmNactivateCallback, showrgbvals,
			   (XtPointer) r);
	    pb = XtVaCreateManagedWidget ("close", xmPushButtonWidgetClass,
					  colormapDialog,
					  XmNtopAttachment,    XmATTACH_WIDGET,
					  XmNtopWidget,	       r,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment,  XmATTACH_FORM,
					  XmNleftAttachment,   XmATTACH_FORM,
					  NULL);
	    XtAddCallback(pb, XmNactivateCallback, popDown, NULL);
	}

	XtManageChild (rc);

    }
    XtManageChild (colormapDialog);
    XmSetColorCalculation (oldproc);
    XUndefineCursor(XtDisplay(w), XtWindow(mainWin));
}



#if 0
Widget personalList, personalListDialog;
void
managePersonalList (w, cl, cd)
Widget w;
XtPointer cl;
XtPointer cd;
{
    Widget add, delete, name;
    XColor rgb;
    static  Widget selectionDialog;

    if (!personalListDialog) {
	selectionDialog =
	    XmCreateSelectionDialog (w, "selectionDialog", NULL, 0);

	personalListDialog =
	    XmCreateFormDialog (w, "personalDialog", NULL, 0);

	personalList = XmCreateScrolledList (personalListDialog,
					     "personalList",
					     NULL, 0);
	XtVaSetValues (XtParent (personalList),
		       XmNtopAttachment,   XmATTACH_FORM,
		       XmNrightAttachment, XmATTACH_FORM,
		       XmNleftAttachment,  XmATTACH_FORM,
		       NULL);


	add = XtVaCreateManagedWidget ("addPB", xmPushButtonWidgetClass,
				       personalListDialog,
				       XmNtopAttachment, XmATTACH_NONE,
				       XmNbottomAttachment, XmATTACH_FORM,
				       NULL);

	delete =
	    XtVaCreateManagedWidget ("deletePB", xmPushButtonWidgetClass,
				     personalListDialog,
				     XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
				     XmNtopWidget, add,
				     XmNleftAttachment, XmATTACH_WIDGET,
				     XmNleftWidget, add,
				     XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
				     XmNbottomWidget, add,
				     NULL);

	name =
	    XtVaCreateManagedWidget ("name", xmTextFieldWidgetClass,
				     personalListDialog,
				     XmNrightAttachment,  XmATTACH_FORM,
				     XmNleftAttachment,   XmATTACH_FORM,
				     XmNtopAttachment,    XmATTACH_WIDGET,
				     XmNtopWidget,	     XtParent (personalList),
				     XmNbottomAttachment, XmATTACH_WIDGET,
				     XmNbottomWidget,     add,
				     NULL);

	XtAddCallback (add, XmNactivateCallback, addToList,
		       (XtPointer) name);

	XtManageChild (personalList);
    }

    XtManageChild (personalListDialog);
    XtManageChild (selectionDialog);
}
#endif

Window chooseWindow (w)
    Widget w;
{
    Window chosen, last, ignorew;
    int     ignore;
    unsigned int    ignoreu;
    XEvent ev;

    XGrabPointer (disp,
		  XRootWindowOfScreen (XDefaultScreenOfDisplay (disp)),
		  FALSE,
		  ButtonPressMask,
		  GrabModeAsync,
		  GrabModeSync,
		  None,
		  XCreateFontCursor (disp, XC_crosshair),
		  CurrentTime);

    while (True) {
	XtAppNextEvent (XtWidgetToApplicationContext (w), &ev);
	if (ev.type == ButtonPress) {
	    break;
	}
	else {
	    XtDispatchEvent (&ev);
	}
    }

    last = XDefaultRootWindow (disp);
    chosen = last;
    while (chosen != None) {
	XQueryPointer (disp, last, &ignorew, &chosen,
		       &ignore, &ignore, &ignore, &ignore, &ignoreu);
	if (chosen != None)
	    last = chosen;
    }
    XUngrabPointer (disp, CurrentTime);
    return last;
}

void
setCurrentColorText(w, xc)
    Widget w;
    XColor *xc;
{
    char buffer[32];
    char *name;
    XColor color;

    color.pixel = xc->pixel;
    XQueryColor(XtDisplay(w), cmap, &color);

    if( (name = matchColor(&color)) != NULL) {
	char *s = strdup(name);
	XmTextFieldSetString(w, s);
        XtFree(s);
    } else {
        sprintf(buffer, "#%.2X%.2X%.2X",
	        color.red / 256,
	        color.green / 256,
	        color.blue / 256);
        XmTextFieldSetString(w, buffer);
    }
}

char *
matchColor(xc)
    XColor xc;
{
     COLORINFOPTR clist = colorList;

     if( clist == NULL ) {
          return(NULL);
     } else {
          while ( clist ) {
		if(clist->xc.red == xc.red/256 && 
			clist->xc.green == xc.green/256 && 
			clist->xc.blue == xc.blue/256) {
	    	return(clist->name);
		}
               clist = clist->next;
          }
     }
   return(NULL);
}

updateEverybody(color)
   XColor *color;
{
	Widget theText;
	char buf[32];

    XtVaGetValues (red,
	    XmNuserData, &theText,
	    NULL);
    sprintf(buf, "%d",  color->red/256);
    XmTextFieldSetString(theText, buf);

    XtVaGetValues (green,
	    XmNuserData, &theText,
	    NULL);
    sprintf(buf, "%d", color->green/256);
    XmTextFieldSetString(theText, buf);

    XtVaGetValues (blue,
	    XmNuserData, &theText,
	    NULL);
    sprintf(buf, "%d", color->blue/256);
    XmTextFieldSetString(theText, buf);

    if (setBack)
	setCurrentColorText(currentBgText, color);
    else
	setCurrentColorText(currentFgText, color);
}
