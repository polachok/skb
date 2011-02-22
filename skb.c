/*
 * See LICENSE file for license details.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>

#define XKB_CTRLS_MASK (XkbAllControlsMask & ~(XkbInternalModsMask | XkbIgnoreLockModsMask))

static int
get_gr_num(Display *dpy, XkbDescPtr kb,
                               int *num_groups) {
    if (XkbGetControls(dpy, XKB_CTRLS_MASK, kb) == Success) {
        *num_groups = kb->ctrls->num_groups;
        XkbFreeControls(kb, XKB_CTRLS_MASK, 0);
        return 0;
    }
    else {
        fprintf(stderr,"XkbGetControls()");
        return 1;
    }
}

static int
get_gr_names(Display *dpy, XkbDescPtr kb, 
                                 int num_groups, char *groups) {
    int status = 1;
    char *name = NULL;
    int i;

    if (XkbGetNames(dpy, XkbGroupNamesMask, kb) != Success) {
        fprintf(stderr,"XkbGetNames()");
        return 1;
    }
  
    for (i = 0; i < num_groups; i++) {
        if (kb->names->groups[i]) {
            if ((name = XGetAtomName(dpy, kb->names->groups[i]))) {
                strncpy(groups+strlen(groups)), name, 3);
		groups[strlen(groups)]=':';
            }
            else {
                fprintf(stderr,"XGetAtomName()");
            }
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
    int num_groups=0;
    char groups[256]="\0";
    char active[256]="\0";
    int active_group=0;
    int old = -1;
    int i=0,j=0,k=0;

    if(!(dpy = XOpenDisplay(0)))
	fprintf(stderr,"skb: cannot open display\n");
    if (!(kb = XkbAllocKeyboard()))
	fprintf(stderr,"XkbAllocKeyboard()");

    if (get_gr_num(dpy, kb, &num_groups))
	fprintf(stderr, "skb: cannot get groups number\n");
    
    if (get_gr_names(dpy, kb, num_groups, groups))
        fprintf(stderr, "skb: cannot get groups names\n");

    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbAllEventsMask, XkbAllEventsMask);
    for(;;) {
        if (get_active_gr(dpy, &active_group))
            printf("skb: cannot get active group\n");
        if(active_group!=old) {
            while(i<strlen(groups))
            {
                if(groups[i]==':')
                {
                    strncpy(active,groups+k,3);
                    k=i+1;
                    j++;
                    if(j==active_group+1)
                      break;
                    bzero(active, 256);
                }
                i++;
            }
            if(strlen(active)==0)
                return 1;
            active[strlen(active)]='\0';
            puts(active);
	    fflush(stdout);
            i=j=k=0;
            old=active_group;
        }
	if(argc > 1)
	    break;
	XNextEvent(dpy,&ev.core);
    };
    //printf("num of gr: %d, active_group: %d, groups: %s act: %s\n", num_groups, active_group, groups, active);
    return 0;
}
