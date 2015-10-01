#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/MessageB.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include "main.h"


Widget helpDialog;
Widget helpIndexDialog;

void createHelpDialog();
char *getMessage();
void helpCB ();
void contextHelp();
void helpListCB();
void manageIndexHelpDialog();
XmString getContextText();

/* this is the help callback for all widgets */
void
helpCB (w, cl, cd)
    Widget w;
    XtPointer cl;
    XtPointer cd;
{
    XmString xms;

    if(( xms = getContextText(w)) != NULL) {
	XtVaSetValues(helpDialog,
		      XmNmessageString, xms,
		      NULL);
    }
    XtManageChild(helpDialog);
}

/* activated from the "On Context" pb in the help menu */
void
contextHelp(w, cl, cd)
	Widget          w;
	XtPointer       cl, cd;
{
     Widget which;
     Cursor cursor;
     XEvent event;
     XmAnyCallbackStruct cbs;
     extern Widget mainWin;

     cursor = XCreateFontCursor( XtDisplay(w), XC_question_arrow);
     /* confine it to mainWin */
     which = XmTrackingEvent(mainWin, cursor, True, &event);

     while(which != NULL) {
	  cbs.reason = XmCR_HELP;
	  cbs.event = &event;
	  if( XtHasCallbacks(which, XmNhelpCallback) == XtCallbackHasSome){
	       XtCallCallbacks(which, XmNhelpCallback, (XtPointer) &cbs);
	       which = NULL;
	  }
	  else
	       which = XtParent(which);
     }

     XFreeCursor(XtDisplay(w), cursor);
}

/* This will look in /usr/lib/X11/help for files named Classname.sufN
       where N in passed in here and Classname is the classname of
       this app and suf is passed in.  The contents of the file are
       returned and must be freed. You can change the directory with
       $XFILESEARCHPATH (but keep the default)

       e.g.
       setenv XFILESEARCHPATH /usr/lib/X11/%L/%T/%N%S:\
       /usr/lib/X11/%l/%T/%N%S:\
       /usr/lib/X11/%T/%N%S:\
       /wherever-you-want/%T/%N%S
*/
char *
getMessage(display, suf, n)
    Display *display;
    char *suf;
    int n;
{
    FILE *fp;
    char *fileName;
    char *helpText;
    long filesize;
    char suffix[64];

    sprintf(suffix, ".%s%d", suf, n);
    if((fileName = XtResolvePathname(display, "help", NULL, suffix, NULL,
				     NULL,0,NULL)) == NULL) {
	fprintf(stderr, "Cannot open help file %s\n", suffix);
	return(NULL);
    }

    if( (fp = fopen(fileName, "r")) == NULL) {
	perror("on open of help file");
	XtFree((char *) fileName);
	return(NULL);
    }
    fseek(fp, 0L, 2);
    filesize = ftell(fp);
    fseek(fp, 0L, 0);

    if ((helpText = (char *)malloc(filesize+1)) == NULL) {
	fclose(fp);
	return(NULL);
    } else {
	fread(helpText, filesize, 1, fp);
    }
    helpText[ftell(fp)] = '\0';

    fclose(fp);
    XtFree(fileName);
    return(helpText);
}

void
helpListCB(w, cl,cd)
     Widget w;
     XtPointer cl,cd;
{
     XmListCallbackStruct *cbs =
	  (XmListCallbackStruct *) cd;
     Widget text = (Widget)cl;
     char *string, oops[128];
     String classname, name; /* dont free these */

     if((string=getMessage(XtDisplay(w), "index", cbs->item_position))!=NULL) {
	 XmTextSetString(text, string);
	 XtFree(string);
     }
     else {
	 XtGetApplicationNameAndClass( XtDisplay(w), &name, &classname);
	 sprintf(oops,
		 "Cannot find help file /usr/lib/X11/help/%s.index%d\n\
Either install it there or try setting XFILESEARCHPATH.",
		 classname,
		 cbs->item_position);
 	 XmTextSetString(text, oops);
     }
}

void
createHelpDialog(parent)
     Widget parent;
{
    Widget text, list, sep, pb;

    helpDialog = XmCreateInformationDialog(parent, "helpDialog" ,NULL,0);
    XtUnmanageChild( XmMessageBoxGetChild(helpDialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild( XmMessageBoxGetChild(helpDialog, XmDIALOG_HELP_BUTTON));

    helpIndexDialog = XmCreateFormDialog(parent, "helpIndexDialog", NULL, 0);
    XtVaSetValues(helpIndexDialog,
		  XmNfractionBase,     10,
		  NULL);

    list = XmCreateScrolledList(helpIndexDialog, "helpList", NULL,0);
    XtVaSetValues(list,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);
    XtVaSetValues(XtParent(list),
		  XmNleftAttachment,	   XmATTACH_FORM,
		  XmNleftOffset,	   20,
		  XmNtopAttachment,	   XmATTACH_FORM,
		  XmNtopOffset,		   10,
		  XmNbottomAttachment,	   XmATTACH_FORM,
		  XmNbottomOffset,	   80,
		  XmNrightAttachment,	   XmATTACH_OPPOSITE_FORM,
		  XmNrightOffset,	   -200,
		  NULL);


    text = XmCreateScrolledText(helpIndexDialog, "helpText", NULL,0);

    XtVaSetValues(text,
		  XmNeditMode,			XmMULTI_LINE_EDIT,
		  XmNeditable,			False,
		  XmNcursorPositionVisible,	False,
		  XmNwordWrap,			True,
		  XmNcolumns,			80,
		  NULL);

    XtVaSetValues(XtParent(text),
		  XmNtopAttachment,	   XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget,		   XtParent(list),
		  XmNleftAttachment,	   XmATTACH_WIDGET,
		  XmNleftWidget,	   XtParent(list),
		  XmNrightAttachment,	   XmATTACH_FORM,
		  XmNbottomAttachment,	   XmATTACH_OPPOSITE_WIDGET,
		  XmNbottomWidget,	   XtParent(list),
		  XmNleftOffset,	   20,
		  XmNrightOffset,	   20,
		  NULL);

    sep = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
				  helpIndexDialog,
				  XmNleftAttachment,	XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_FORM,
				  XmNbottomAttachment,	XmATTACH_FORM,
				  XmNbottomOffset,	60,
				  NULL);

    pb = XtVaCreateManagedWidget("close", xmPushButtonWidgetClass,
				 helpIndexDialog,
				 XmNbottomAttachment,	  XmATTACH_FORM,
				 XmNbottomOffset,	  10,
				 XmNleftAttachment,	  XmATTACH_POSITION,
				 XmNleftPosition,	  4,
				 XmNrightAttachment,	  XmATTACH_POSITION,
				 XmNrightPosition,	  6,
				 XmNtopAttachment,	  XmATTACH_WIDGET,
				 XmNtopWidget,		  sep,
				 XmNtopOffset,		  10,
				 NULL);


    XtManageChild(text);
    XtManageChild(list);
    XtAddCallback(list, XmNsingleSelectionCallback, helpListCB, text);
}

void
manageIndexHelpDialog()
{
    XtManageChild(helpIndexDialog);
}

XmString
getContextText(w)
    Widget w;
{
    static XmString value;
    static XtResource resources[] = {
	{"text","Text", XmRXmString, sizeof(XmString),
	 0, XmRXmString, (XtPointer) "Cannot get subresources"}
    };

    if(appdata.debug)
	printWidgetName(w);

    XtGetSubresources(w, &value, "help", "Help", resources,
		      1, NULL, 0);
    return(value);
}

printWidgetName(w)
    Widget w;
{
    Widget wid;
    char **theName=NULL;
    int j,n;

    for( wid = w, n=0; XtParent(wid); wid = XtParent(wid), n++ ) {
	theName = (char **)XtRealloc( (char *)theName, sizeof(char *) * (n+1) );
	theName[n] = strdup( XtName(wid) );
    }
    printf("%s.", XtName(wid) );
    for(j=n-1; j>0; j--) {
	printf("%s.", theName[j] );
	XtFree((char *) theName[j]);
    }
    printf("%s\n", theName[0] );
    XtFree((char *) theName[0]);

    XtFree((char *) theName);
}
