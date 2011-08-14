RM?=	rm
LDFLAGS+=	-lz
PREFIX?=	/usr/local

all:	mkfwimage

install:
	install -m 0755 mkfwimage ${PREFIX}/bin

mkfwimage: mkfwimage.c fw.h

clean:
	$(RM) -f mkfwimage *.o
