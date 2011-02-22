# skb - simple XKB status displayer
# (c) Alexander Polakov

include config.mk

SKBSRC = skb.c xkb.c
XSKBSRC = xskb.c xkb.c
SKBOBJ = ${SKBSRC:.c=.o}
XSKBOBJ = ${XSKBSRC:.c=.o}

all: options skb xskb

options:
	@echo skb build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo "LD       = ${LD}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

skb: ${SKBOBJ}
	@echo LD $@
	@${LD} -o $@ ${SKBOBJ} ${LDFLAGS}
	@strip $@

xskb: ${XSKBOBJ}
	@echo LD $@
	@${LD} -o $@ ${XSKBOBJ} ${LDFLAGS}
	@strip $@

clean: 
	@echo cleaning
	@rm -f skb ${SKBOBJ} ${XSKBOBJ}

dist: clean
	@echo creating dist tarball
	@mkdir -p skb-${VERSION}
	@cp -R LICENSE Makefile README config.mk \
	skb.c skb-${VERSION}
	@tar -cf skb-${VERSION}.tar skb-${VERSION}
	@gzip skb-${VERSION}.tar
	@rm -rf skb-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f skb ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/skb

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/skb
	
.PHONY: all options clean install uninstall
