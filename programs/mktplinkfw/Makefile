RM?=	rm
LDFLAGS+=	-lz -lcrypto
PREFIX?=	/usr/local

all:	mktplinkfw

install:
	install -m 0755 mktplinkfw ${PREFIX}/bin

mkfwimage: mktplinkfw.c

clean:
	$(RM) -f mktplinkfw *.o
