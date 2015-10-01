#include "colorinfo.h"
#include <stdio.h>
#include <string.h>

COLORINFOPTR getNthColorInfo();

COLORINFOPTR colorList=NULL;
COLORINFOPTR currentColor=NULL;
void printList();


/*****************************************************************/
XColor *
getColorFromList(pos)
int pos;
{
	COLORINFOPTR p;
	p = getNthColorInfo(colorList, pos);
	return( &(p->xc) );
}
/*****************************************************************/
deleteFromColorList(pos)
	int pos;
{
	COLORINFOPTR p, prev, next;
	p = getNthColorInfo(colorList, pos);

	prev = p->prev;
	next = p->next;
	if(prev)
		prev->next = next;
	if(next)
		next->prev = prev;
	XtFree(p->name);
	XtFree(p);
	p->name = NULL;
	p = NULL;
	if(next == NULL && prev == NULL)
		colorList = NULL;
}
/*****************************************************************/
addToColorList(xc, name)
	XColor *xc;
	char *name;
{
	COLORINFOPTR color = newColorInfo();

	color->xc.red = xc->red;
	color->xc.green = xc->green;
	color->xc.blue = xc->blue;
	color->xc.flags = xc->flags;
	color->name = (char *)malloc(sizeof(char) * strlen(name));
	strcpy(color->name, name);
	if(colorList == NULL)
		colorList = insertColorInfo(&colorList, color);
	else
		insertColorInfo(&colorList, color);
}


/*****************************************************************/
COLORINFOPTR
newColorInfo()
{
     COLORINFOPTR p = NULL;
     if ((p = (COLORINFOPTR) malloc(sizeof(COLORINFO))) == NULL) {
	  return(NULL);
     }

     p->next = NULL;
     p->prev = NULL;
     p->name = NULL;

     return(p);
}
/*****************************************************************/
COLORINFOPTR
insertColorInfo(list, node)
     COLORINFOPTR *list;
     COLORINFOPTR node;
{
     COLORINFOPTR p = *list;

     if(p == NULL) {
	  p = node;
	  p->prev = NULL;
	  return(p);
     }

     if(p == node) {
	  return(p);
     } else {
	  /* go to the end of the list */
	  while ( p->next ) {
	       p = p->next;
	  }
	  node->prev = p;
	  p->next = node;
	  return(p);
     }

}
/*****************************************************************/
printColors(fp)
     FILE *fp;
{
     printList( fp, colorList );
}

/*****************************************************************/
void
printList(fp, list)
     FILE *fp;
     COLORINFOPTR list;
{
     if( list == NULL ) {
	  return;
	
     } else {
	  while ( list ) {
	       printInfoNode(fp, list);
	       list = list->next;
	  }
     }
}
/*****************************************************************/
printInfoNode(fp, node)
     FILE *fp;
     COLORINFOPTR node;
{
        fprintf (fp, "%-3d %-3d %-3d\t\t%s\n",
	node->xc.red, node->xc.green, node->xc.blue, node->name);
}

/*****************************************************************/
COLORINFOPTR
getNthColorInfo(list, nth)
     COLORINFOPTR list;
     int nth;
{
     int count=0;

     if( list == NULL ) {
	  return;
	
     } else {
	  while ( list && count != nth) {
	       count++;
	       list = list->next;
	  }
     }
     return(list);
}

/*****************************************************************/
readColorInfo(fp)
     FILE *fp;
{
     char buffer[255], buff2[512];
     char *ptr, *p, *p2;
     COLORINFOPTR mmp = newColorInfo();

     while( (ptr = fgets(buffer, 255, fp)) != NULL) {
     }
}
/*****************************************************************/
