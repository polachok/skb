#define MAXGROUPLENGTH 256
#define OUTPUT_LENGTH 3
#define XKB_CTRLS_MASK (XkbAllControlsMask & ~(XkbInternalModsMask | XkbIgnoreLockModsMask))

int get_gr_num(Display *dpy, XkbDescPtr kb);
void get_gr_names(Display *dpy, XkbDescPtr kb, int ngroups, char **groups);
int get_active_gr(Display *dpy);
void enable_gr_events(Display *dpy);
void wait_gr_event(Display *dpy);
