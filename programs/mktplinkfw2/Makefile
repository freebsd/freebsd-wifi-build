RM?=	rm
LDFLAGS+=	-lz -lcrypto
PREFIX?=	/usr/local

all:	mktplinkfw2

install:
	install -m 0755 mktplinkfw2 ${PREFIX}/bin

mkfwimage: mktplinkfw2.c

clean:
	$(RM) -f mktplinkfw2 *.o
