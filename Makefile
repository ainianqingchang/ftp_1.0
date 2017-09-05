
all:cli serv

#witch compiler
CC=clang

#where are Include files kept
INCLUDE=.

CFLAGS=

LIBS=-L.

cli:cli.o menucli.o cmdstr.o 
	$(CC) $(LIBS) -o cli cli.o menucli.o cmdstr.o -lunp 

serv:serv.o menuserv.o cmdstr.o
	$(CC) $(LIBS) -D_REENTRANT -o serv serv.o menuserv.o cmdstr.o -lunp -lpthread

cli.o:cli.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c cli.c
menuserv.o:menuserv.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c menuserv.c

menucli.o:menucli.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c menucli.c

cmdstr.o:cmdstr.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c cmdstr.c
serv.o:serv.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c serv.c

clean:
	-rm *.o
