/*
 * See LICENSE file for license details.
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>

#define XKB_CTRLS_MASK (XkbAllControlsMask & ~(XkbInternalModsMask | XkbIgnoreLockModsMask))

void
eprint(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

static int
get_gr_num(Display *dpy, XkbDescPtr kb) {
    int rv;

    if (XkbGetControls(dpy, XKB_CTRLS_MASK, kb) != Success)
	eprint("XkbGetControls() failed.\n");
    rv = kb->ctrls->num_groups;
    XkbFreeControls(kb, XKB_CTRLS_MASK, 0);
    return rv;
}

static int
get_gr_names(Display *dpy, XkbDescPtr kb, 
                                 int num_groups, char **groups) {
    int status = 1;
    char *name = NULL;
    int i;

    if (XkbGetNames(dpy, XkbGroupNamesMask, kb) != Success)
        eprint("XkbGetNames() failed");
  
    for (i = 0; i < num_groups; i++) {
        if (kb->names->groups[i]) {
            if ((name = XGetAtomName(dpy, kb->names->groups[i])))
		snprintf(groups[i], 256, name);
            else
		eprint("XGetAtomName() failed\n");
        }
        else {
	    fprintf(stderr, "Something weird happened");
        }
    }
    status = 0;
    XkbFreeNames(kb, XkbGroupNamesMask, 0);
    return status;
}

static int
get_active_gr(Display *dpy, int *active_group) {
    XkbStateRec state;

    if (XkbGetState(dpy, XkbUseCoreKbd, &state) == Success) {
        *active_group = state.group;        
        return 0;
    }
    else {
        fprintf(stderr,"XkbGetState()");
        return 1;
    }
}

int
main(int argc, char *argv[]){
    Display *dpy;
    XkbDescPtr kb;
    XkbEvent ev;
    int num_groups = 0;
    char **groups;
    int active_group = 0;
    int old = -1;
    int i, j, k;

    i = j = k = 0;
    if(!(dpy = XOpenDisplay(0)))
	eprint("skb: cannot open display\n");

    if (!(kb = XkbAllocKeyboard()))
	eprint("XkbAllocKeyboard()\n");

    num_groups = get_gr_num(dpy, kb);
    groups = malloc(sizeof(char*)*num_groups);
    for (i = 0; i < num_groups; i++)
	    groups[i] = malloc(256); 
    
    if (get_gr_names(dpy, kb, num_groups, groups))
        eprint("skb: cannot get groups names\n");

    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbAllEventsMask, XkbAllEventsMask);
    for(;;) {
        if (get_active_gr(dpy, &active_group))
            eprint("cannot get active group\n");
        if(active_group != old) {
            puts(groups[active_group]);
	    fflush(stdout);
            old=active_group;
        }
	if(argc > 1)
	    break;
	XNextEvent(dpy,&ev.core);
    };
    //printf("num of gr: %d, active_group: %d, groups: %s act: %s\n", num_groups, active_group, groups, active);
    return 0;
}
