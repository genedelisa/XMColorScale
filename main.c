/* Gene De Lisa */
/* gene@bluestone.com */

#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Protocols.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include "main.h"
#include "help.h"

extern Widget list;
#ifdef _NO_PROTO
#else
XtEventHandler onTop( Widget w, XtPointer cl, XEvent *ev, Boolean *cont);
#endif

void popDown();
Boolean isColorVisual();
Widget          menuBar;
Widget          mainWinWw;
Widget          mainWin;
Widget          filePulldown;
Widget          createMainWindow();
Display        *display = NULL;
int badColorName = 0;


#if (XtSpecificationRelease >= 5)
extern void     _XEditResCheckMessages();
#endif

Atom            wm_delete_window;
String          fallbacks[] = {"*fontList:9x15=charset",
NULL};

void             reallyQuit();
void             quitCB();

#if (XmVersion<1002)

#define XmFONTLIST_DEFAULT_TAG XmSTRING_DEFAULT_CHARSET

XmString
XmStringCreateLocalized(str)
	char *str;
{
    return(XmStringCreateSimple(str));
}

#endif

#ifdef XOPEN_CATALOG
#include <nl_types.h>
readMessageCatalog()
{
    nl_catd catd;
    int set_num = 0;
    int messageNum;

    if ( (catd = catopen("testcat", 0)) == -1) {
	fprintf(stderr, "cannot open cat\n");
    }

    for(messageNum=0; messageNum < 10; messageNum++)
	printf("<%s>\n",
	       catgets(catd, set_num, messageNum, "default message") );

    catclose(catd);
}
#endif

/* you cannot make any Xlib calls in here */
errorHandler(d,e)
Display *d;
XErrorEvent *e;
{
    char msg[80];

    fprintf(stderr, "Error: error %d request %d minor %d serial %d type %d\n",
	    e->error_code,
	    e->request_code,
	    e->minor_code,
	    e->serial,
	    e->type);

    XGetErrorText(d, e->error_code, msg, 80);
    fprintf(stderr, "Protocol Error: %s\n", msg);

    if(e->error_code == 15 && e->request_code == 90) {
	badColorName = 1;
    }

    return(0);
}

AppData appdata;

#define XmNdebug "debug"
#define XmCdebug "Debug"

#define XmNrgbFile "rgbFile"
#define XmCrgbFile "RgbFile"

#define XmNpersonalRgbFile "personalRgbFile"
#define XmCpersonalRgbFile "PersonalRgbFile"

#define XmNgotResfile "gotResfile"
#define XmCGotResfile "GotResfile"

#define XmNgotColorResfile "gotColorResfile"
#define XmCGotColorResfile "GotColorResfile"

#define XmNforceNoncolor "forceNoncolor"
#define XmCForceNoncolor "ForceNoncolor"

#include <X11/StringDefs.h>

XtResource appRes[] =
{
    {XmNgotResfile,XmCGotResfile,XtRBoolean,sizeof(Boolean),
     XtOffsetOf(AppData,gotResfile), XtRImmediate, (XtPointer)False},

    {XmNgotColorResfile,XmCGotColorResfile,XtRBoolean,sizeof(Boolean),
     XtOffsetOf(AppData,gotColorResfile), XtRImmediate, (XtPointer)False},

    {XmNdebug,XmCdebug,XtRBoolean,sizeof(Boolean),
     XtOffsetOf(AppData,debug), XtRImmediate, (XtPointer)False},

    {XmNrgbFile,XmCrgbFile,XtRString,255,
     XtOffsetOf(AppData,rgbFile), XtRString,
     (XtPointer)"/usr/lib/X11/rgb.txt"},

    {XmNpersonalRgbFile,XmCpersonalRgbFile,XtRString,255,
     XtOffsetOf(AppData,personalRgbFile), XtRString,
     (XtPointer)NULL},

    {XmNforceNoncolor,XmCForceNoncolor,XtRBoolean,sizeof(Boolean),
     XtOffsetOf(AppData,forceNoncolor), XtRImmediate, (XtPointer)False}
};
XrmOptionDescRec options[] = {
{"-debug", "*debug", XrmoptionNoArg, "True"},
{"-rgbfile", "*rgbFile", XrmoptionSepArg, "/usr/lib/X11/rgb.txt"},
{"-personalRgbfile", "*personalRgbFile", XrmoptionSepArg, NULL}
};

Boolean
work(cl)
	XtPointer cl;
{
static Widget   dialog=NULL;
Widget          b;
XmString        xs;

if (!dialog) {
    /*
     * the message box is returned, its parent is the DialogShell
     */
    dialog = XmCreateErrorDialog(mainWin, "oops",
				 NULL, 0);
    XtVaSetValues(XtParent(dialog),
		  XmNtitle,     "Protocol Error",
		  NULL);

    b = XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(b);
    b = XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(b);
}

if(badColorName == 1) {
    badColorName = 0;
    xs = XmStringCreateLocalized("Protocol error: Bad Color Name");
    XtVaSetValues(dialog,
		  XmNmessageString,        xs,
		  XmNmessageAlignment,     XmALIGNMENT_CENTER,
		  NULL);
    XmStringFree(xs);
    XtManageChild(dialog);
}

return(False);
}

main(argc, argv)
	int             argc;
	char          **argv;
{
    XtAppContext    app;
    Widget          shell, mainWin;
    XtWorkProcId    id;

    XtSetLanguageProc(NULL,(XtLanguageProc)NULL,NULL);

#ifdef XOPEN_CATALOG
    printf("xopen\n");
    readMessageCatalog();
#endif

    shell = XtVaAppInitialize(&app, "Xmcolorscale",
			      options, XtNumber(options),
			      &argc, argv,
			      fallbacks,
			      XmNdeleteResponse,	   XmDO_NOTHING,
			      XmNallowShellResize,	   True,
			      NULL);

    XtGetApplicationResources(shell, &appdata, appRes, XtNumber(appRes),
			      NULL,0);

    if(appdata.gotResfile == False) {
	exitDialog(app, shell, "No Resource File!");
    }

    if( isColorVisual(shell) == False) {
		if(appdata.forceNoncolor == False)
			exitDialog(app, shell,
		   		"This needs to be run on a color display (of course!)");
    }

    if( isColorVisual(shell) && appdata.gotColorResfile == False) {
	fprintf(stderr,
		"Cannot find resource file for colors! (Xmcolorscale-color)\n");
	fprintf(stderr,
		"(Have you set *customization: -color in your xrdb file?)\n ");
    }


    display = XtDisplay(shell);
    wm_delete_window =
	XmInternAtom(XtDisplay(shell),
		     "WM_DELETE_WINDOW", False);

    mainWin = createMainWindow(shell);
    setupColors(shell);
    createWidgets(mainWin, app);

#if (XtSpecificationRelease >= 5)
    XtAddEventHandler(shell, (EventMask) 0, True,
		      _XEditResCheckMessages, NULL);
#endif

    XtRealizeWidget(shell);
    XmAddWMProtocolCallback(shell, wm_delete_window,
			    reallyQuit, (XtPointer)NULL);
    XSetErrorHandler(errorHandler);

    id = XtAppAddWorkProc(app, work, NULL);

    XtAppMainLoop(app);
}

Widget
createMainWindow(topLevel)
	Widget          topLevel;
{

    mainWin = XtVaCreateManagedWidget("mainWin",
				      xmFormWidgetClass, topLevel,
				      NULL);

    menuBar = XmCreateMenuBar(mainWin, "menuBar", NULL, 0);
    createFileMenu(menuBar);
    createHelpMenu(menuBar);
    XtManageChild(menuBar);
	
    XtVaSetValues(menuBar,
		  XmNtopAttachment,   XmATTACH_FORM,
		  XmNleftAttachment,  XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  NULL);
    return (mainWin);
}

void
quitCB(w, cl, cd)
	Widget          w;
	XtPointer       cl, cd;
{
     exit(0);
}


XtEventHandler
onTop(w, cl, ev, cont)
    Widget w;
    XtPointer cl;
    XEvent *ev;
    Boolean *cont;
{
    XRaiseWindow(XtDisplay(w), XtWindow(w) );
}
void
reallyQuit(w, cl, cd)
	Widget          w;
	XtPointer       cl, cd;
{
    static Widget   dialog;
    Widget          b;
    XmString        xs;

    if (!dialog) {
	xs = XmStringCreateLocalized("Really quit?");
	/*
	 * the message box is returned, its parent is the DialogShell
	 */
	dialog = XmCreateQuestionDialog(mainWin, "reallyQuit",
					NULL, 0);

	/* be obnoxious and keep it on top of the stacking order*/
	XtAddEventHandler(XtParent(dialog),
			  VisibilityChangeMask, False, (XtEventHandler)onTop, NULL);
	
	XtVaSetValues(dialog,
		      XmNmessageString,	       xs,
		      XmNmessageAlignment,     XmALIGNMENT_CENTER,
		      NULL);
	XmStringFree(xs);
	XtVaSetValues(XtParent(dialog),
		      XmNtitle,	    "Really?",
		      NULL);

	XtAddCallback(dialog, XmNokCallback, quitCB, (XtPointer)NULL);

	b = XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(b);
    }
    XtManageChild(dialog);
}
void
readFile(w,cl,cd)
     Widget w;
     XtPointer cl,cd;
{
    XmFileSelectionBoxCallbackStruct *cb =
	(XmFileSelectionBoxCallbackStruct *) cd;
    char *filename;
    Widget d = (Widget)cl;

    if(!XmStringGetLtoR(cb->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
	return;
    }

    fillList(list, filename);


    XtFree(filename);
    XtUnmanageChild(d);
}
openFile(w,cl,cd)
     Widget w;
     XtPointer cl,cd;
{
    static Widget fsd;
    if(!fsd) {
	fsd = XmCreateFileSelectionDialog(w, "openDialog", NULL,0);
	XtAddCallback(fsd, XmNokCallback, readFile, (XtPointer)fsd);
	XtAddCallback(fsd, XmNcancelCallback, popDown,
		      (XtPointer)NULL);
    }
    XtManageChild(fsd);
}
void
writeFile(w,cl,cd)
     Widget w;
     XtPointer cl,cd;
{
    XmFileSelectionBoxCallbackStruct *cb =
	(XmFileSelectionBoxCallbackStruct *) cd;
    char *filename;
    Widget d = (Widget)cl;

    if(!XmStringGetLtoR(cb->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
	return;
    }

    saveList( list, filename );
    XtFree(filename);
    XtUnmanageChild(d);
}
saveFile(w,cl,cd)
     Widget w;
     XtPointer cl,cd;
{
    static Widget fsd;
    if(!fsd) {
	fsd = XmCreateFileSelectionDialog(w, "saveDialog", NULL,0);
	XtAddCallback(fsd, XmNokCallback, writeFile, (XtPointer)fsd);
	XtAddCallback(fsd, XmNcancelCallback,
		      popDown, (XtPointer)NULL);
    }
    XtManageChild(fsd);
}

createFileMenu(parent)
	Widget          parent;
{
    Widget          cascade, quitPB, openCB, openPB, openPulldown;
    Widget savePulldown, saveCB, savePB;


    filePulldown = XmCreatePulldownMenu(parent, "mainWinFilePulldown",
					NULL, 0);
#if 1
    openPulldown = XmCreatePulldownMenu(filePulldown, "fileOpenPulldown",
					NULL, 0);
    savePulldown = XmCreatePulldownMenu(filePulldown, "fileOpenPulldown",
					NULL, 0);

    openCB = XtVaCreateManagedWidget("fileOpenCascade",
				     xmCascadeButtonWidgetClass,
				     filePulldown,
				     XmNsubMenuId,	openPulldown,
				     NULL);

    openPB = XtVaCreateManagedWidget("openPB", xmPushButtonWidgetClass,
				     openPulldown,
				     NULL);
    XtAddCallback(openPB, XmNactivateCallback, (XtCallbackProc)openFile, NULL);

    saveCB = XtVaCreateManagedWidget("fileSaveCascade",
				     xmCascadeButtonWidgetClass,
				     filePulldown,
				     XmNsubMenuId,	savePulldown,
				     NULL);

    savePB = XtVaCreateManagedWidget("savePB", xmPushButtonWidgetClass,
				     savePulldown,
				     NULL);
    XtAddCallback(savePB, XmNactivateCallback, (XtCallbackProc)saveFile, NULL);
#endif
    quitPB = XtVaCreateManagedWidget("quitPB", xmPushButtonWidgetClass,
				     filePulldown,
				     NULL);
    XtAddCallback(quitPB, XmNactivateCallback, (XtCallbackProc)reallyQuit,(XtPointer) NULL);

    cascade = XtVaCreateManagedWidget("mainWinFileCascade",
				      xmCascadeButtonWidgetClass, parent,
				      XmNsubMenuId,	 filePulldown,
				      NULL);



#if (XmVersion>=1002)
    XtVaSetValues(filePulldown,
		  XmNtearOffModel,	XmTEAR_OFF_ENABLED,
		  NULL);
#if 1
    XtVaSetValues(openPulldown,
		  XmNtearOffModel,	XmTEAR_OFF_ENABLED,
		  NULL);
    XtVaSetValues(savePulldown,
		  XmNtearOffModel,	XmTEAR_OFF_ENABLED,
		  NULL);
#endif
#endif
}

void
versionHelp(w, cl, cd)
	Widget          w;
	XtPointer       cl, cd;
{
    static Widget   dialog;
    Widget          b;
    XmString        msg, xs1, xs2, sep;
    Pixmap   thePix;
    Display *display = XtDisplay(w);
    Screen *screen = XtScreen(w);

    thePix = XmGetPixmap(screen, "mailfull",
			 BlackPixelOfScreen( screen ),
			 WhitePixelOfScreen( screen ) );

    if (!dialog) {
	xs1 = XmStringSegmentCreate("Motif Color Scales",
				    XmFONTLIST_DEFAULT_TAG,
				    XmSTRING_DIRECTION_L_TO_R,
				    True);	/* true means create a
						 * separator */
	/* put int TWO newlines */
	sep = XmStringSeparatorCreate();
	msg = XmStringConcat(xs1, sep);
	xs2 = XmStringCreateLocalized("Gene De Lisa");
	msg = XmStringConcat(msg, xs2);
	XmStringFree(xs1);
	XmStringFree(xs2);
	XmStringFree(sep);
	/*
	 * the message box is returned, its parent is the DialogShell
	 */
	dialog = XmCreateMessageDialog(mainWin, "version", NULL, 0);
	XtVaSetValues(dialog,
		      XmNmessageString,	       msg,
		      XmNmessageAlignment,     XmALIGNMENT_CENTER,
		      XmNsymbolPixmap,	       thePix,
		      XmNbackground,	       WhitePixel(display, 0),
		      NULL);
	XmStringFree(msg);
	XtVaSetValues(XtParent(dialog),
		      XmNtitle,	    "Version",
		      NULL);
	b = XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(b);
	b = XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(b);
    }
    XtManageChild(dialog);
}
createHelpMenu(parent)
	Widget          parent;
{
    Widget          pulldown, cascade, versionPB, contextPB, pb;

    pulldown = XmCreatePulldownMenu(parent, "mainWinHelpPulldown",
				    NULL, 0);

#if (XmVersion>=1002)
    XtVaSetValues(pulldown,
		  XmNtearOffModel,	   XmTEAR_OFF_ENABLED,
		  NULL);
#endif
    contextPB = XtVaCreateManagedWidget("contextPB",
					xmPushButtonWidgetClass,
					pulldown,
					NULL);
    XtAddCallback(contextPB, XmNactivateCallback, (XtCallbackProc)contextHelp,
		  (XtPointer) NULL);
	
    pb = XtVaCreateManagedWidget("indexPB",
				 xmPushButtonWidgetClass,
				 pulldown,
				 NULL);
    XtAddCallback(pb, XmNactivateCallback, manageIndexHelpDialog,
		  (XtPointer) NULL);

    versionPB = XtVaCreateManagedWidget("versionPB",
					xmPushButtonWidgetClass,
					pulldown,
					NULL);
    XtAddCallback(versionPB, XmNactivateCallback, versionHelp,
		  (XtPointer) NULL);

    cascade = XtVaCreateManagedWidget("mainWinHelpCascade",
				      xmCascadeButtonWidgetClass, parent,
				      XmNsubMenuId,	    pulldown,
				      NULL);
    XtVaSetValues(parent,
		  XmNmenuHelpWidget, cascade,
		  NULL);
}

Boolean
isColorVisual(w)
	Widget w;
{
    Display *display = XtDisplay(w);
    int     screen = DefaultScreen(display);
    int     defaultDepth = DefaultDepth(display, screen);
    XVisualInfo     visualInfo;

    if (XMatchVisualInfo(display, screen, defaultDepth,
			 PseudoColor, &visualInfo)) {
/*	printf("PseudoColor");*/
	return(True);
    }

    if (XMatchVisualInfo(display, screen, defaultDepth,
			 DirectColor, &visualInfo)) {
/*	printf("DirectColor");*/
	return(True);

    }
    return(False);
}

byebye()
{
    exit(0);
}
exitDialog(app, shell, string)
    XtAppContext    app;
    Widget shell;
    char *string;
{
    Widget b;
    XmString xms, title;
	
    Widget dialog = XmCreateErrorDialog(shell, "noResourceDialog",
					NULL, 0);
    xms = XmStringCreateLocalized(string);
    title = XmStringCreateLocalized("Resource File Problem!");
    XtVaSetValues(dialog,
		  XmNdialogTitle,	title,
		  XmNmessageString,	xms,
		  NULL);
    XtAddCallback(dialog, XmNokCallback, (XtCallbackProc)byebye, NULL);
    b = XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(b);
    b = XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(b);
    XtRealizeWidget(dialog);
    XtManageChild(dialog);	
    XtAppMainLoop(app);
}
