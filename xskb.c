/*
 * See LICENSE file for license details.
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xresource.h>
#include "skb.h"

Window win;
XrmDatabase xrdb;
XFontStruct *font;
int screen;
int width = 30;
int height = 12;
int x;
int y;

void
eprint(const char *errstr, ...)
{
    va_list ap;

    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void
run(Display *dpy, char **groups, XColor *colors) {
    int active = 0;
    int old = -1;
    int th, ty, tw;
    GC gc;

    th = font->ascent + font->descent;
    ty = (height / 2) - (th / 2) + font->ascent;
    gc = XCreateGC(dpy, DefaultRootWindow(dpy), 0, 0);
    XSetFont(dpy, gc, font->fid);
    for(;;) {
        active = get_active_gr(dpy);
	if(active != old) {
	    XSetForeground(dpy, gc, colors[active].pixel);
	    XFillRectangle(dpy, win, gc, 0, 0, width, height);
	    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
	    tw = XTextWidth(font, groups[active], strlen(groups[active]));
	    XDrawString(dpy, win, gc, width/2 - tw/2, ty, groups[active], strlen(groups[active]));
	    old = active;
	}
	wait_gr_event(dpy);
    };
}

char *
getresource(const char *resource, char *defval) {
   static char name[256], class[256], *type;
   XrmValue value;
#define RESNAME "xskb"
#define RESCLASS "XSkb"
   snprintf(name, sizeof(name), "%s.%s", RESNAME, resource);
   snprintf(class, sizeof(class), "%s.%s", RESCLASS, resource);
   XrmGetResource(xrdb, name, class, &type, &value);
   if(value.addr)
           return value.addr;
   return defval;
}

int
main(int argc, char *argv[]){
    Display *dpy;
    XkbDescPtr kb;
    XSetWindowAttributes xwa;
    XColor *colors;
    Colormap cmap;
    int ngroups = 0;
    char **groups;
    char *colstr, *xrms, *home, *fontstr;
    char resname[20];
    char tmp[256];
    int ch, i;

    /* parse command line */
    while ((ch = getopt(argc, argv, "x:y:w:h:")) != -1) {
	switch (ch) {
	    case 'x':
		x = atoi(optarg);
		break;

	    case 'y':
		y = atoi(optarg);
		break;

	    case 'w':
		width = atoi(optarg);
		break;

	    case 'h':
		height = atoi(optarg);
		break;

	    default:
		eprint("usage: xskb [-x x] [-y y] [-w width] [-h height]");
	}
    }

    if(!(dpy = XOpenDisplay(0)))
	eprint("skb: cannot open display\n");

    if (!(kb = XkbAllocKeyboard()))
	eprint("skb: XkbAllocKeyboard()\n");

    home = getenv("HOME");
    snprintf(tmp, sizeof(tmp), "%s/.Xdefaults", home);
    XrmInitialize();
    if(!(xrdb = XrmGetDatabase(dpy))) {
	if((xrms = XResourceManagerString(dpy)))
	    xrdb = XrmGetStringDatabase(xrms);
	if(!xrdb)
	    xrdb = XrmGetFileDatabase(tmp);
    }
    fontstr = getresource("font", "fixed");
    if(!(font = XLoadQueryFont(dpy, fontstr)))
	eprint("skb: XLoadQueryFont failed for %s\n", fontstr);
    screen = DefaultScreen(dpy);
    cmap = DefaultColormap(dpy, screen);
    xwa.override_redirect = 1;
    xwa.background_pixel = BlackPixel(dpy, screen);
    win = XCreateWindow(dpy, DefaultRootWindow(dpy),
	    x, y, width, height, 0, CopyFromParent,
	    InputOutput,
	    DefaultVisual(dpy, screen),
	    CWBackPixel|CWOverrideRedirect, &xwa);
    XMapWindow(dpy, win);

    ngroups = get_gr_num(dpy, kb);

    colors = malloc(sizeof(XColor)*ngroups);
    groups = malloc(sizeof(char*)*ngroups);
    for (i = 0; i < ngroups; i++) {
	snprintf(resname, sizeof(resname), "color%d", i); 
	colstr = getresource(resname, "red");
	groups[i] = malloc(MAXGROUPLENGTH); 
	if(!XAllocNamedColor(dpy, cmap, colstr, &colors[i], &colors[i]))
	    eprint("error, cannot allocate color '%s'\n", colstr);
    }
    
    get_gr_names(dpy, kb, ngroups, groups);
    enable_gr_events(dpy);
    run(dpy, groups, colors);
    return 0;
}
