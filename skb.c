/*
 * See LICENSE file for license details.
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include "skb.h"

void
eprint(const char *errstr, ...)
{
    va_list ap;

    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[]){
    Display *dpy;
    XkbDescPtr kb;
    int ngroups = 0;
    char **groups;
    int active = 0;
    int old = -1;
    int i;

    if(!(dpy = XOpenDisplay(0)))
	eprint("skb: cannot open display\n");

    if (!(kb = XkbAllocKeyboard()))
	eprint("skb: XkbAllocKeyboard()\n");

    ngroups = get_gr_num(dpy, kb);

    groups = malloc(sizeof(char*)*ngroups);
    for (i = 0; i < ngroups; i++)
	    groups[i] = malloc(MAXGROUPLENGTH); 
    
    get_gr_names(dpy, kb, ngroups, groups);
    enable_gr_events(dpy);
    for(;;) {
        active = get_active_gr(dpy);
        if(active != old) {
            puts(groups[active]);
	    fflush(stdout);
            old = active;
        }
	if(argc > 1)
	    break;
	wait_gr_event(dpy);
    };
    return 0;
}
