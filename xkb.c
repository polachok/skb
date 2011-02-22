/*
 * See LICENSE file for license details.
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include "skb.h"

int
get_gr_num(Display *dpy, XkbDescPtr kb) {
    int rv;

    if (XkbGetControls(dpy, XKB_CTRLS_MASK, kb) != Success)
	eprint("skb: XkbGetControls() failed.\n");
    rv = kb->ctrls->num_groups;
    XkbFreeControls(kb, XKB_CTRLS_MASK, 0);
    return rv;
}

void
get_gr_names(Display *dpy, XkbDescPtr kb, int ngroups, char **groups) {
    char *name = NULL;
    int i;

    if (XkbGetNames(dpy, XkbGroupNamesMask, kb) != Success)
        eprint("skb: XkbGetNames() failed");
  
    for (i = 0; i < ngroups; i++) {
        if (kb->names->groups[i]) {
            if ((name = XGetAtomName(dpy, kb->names->groups[i])))
		snprintf(groups[i], OUTPUT_LENGTH+1, name);
            else
		eprint("skb: XGetAtomName() failed\n");
        }
    }
    XkbFreeNames(kb, XkbGroupNamesMask, 0);
}

int
get_active_gr(Display *dpy) {
    XkbStateRec state;

    if (XkbGetState(dpy, XkbUseCoreKbd, &state) != Success)
	eprint("skb: XkbGetState() failed\n");
    return state.group;        
}

void
enable_gr_events(Display *dpy) {
    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbAllEventsMask, XkbAllEventsMask);
}

void
wait_gr_event(Display *dpy) {
    XkbEvent ev;
    XNextEvent(dpy, &ev.core);
}
