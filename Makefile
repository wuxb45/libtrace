.PHONY : all clean
COMMONDEP := libtrace3.h libtrace2.h libtrace.h Makefile
#OPT := -ggdb -O0
#OPT := -O3
OPT := -Wall -Wextra -mtune=native -march=native -O3 -flto

BINNAMES := arc lirs lrux lru mergesizes replay \
    op42spx op4print op4stat op4genzipf op42keys \
    orig2op4 origkeymap origprint origstat origstatget origstattype origvlen origkvlen \
    spx2op4 spxgenzipf spxstat spxstatv \
    u64sort vlengen vlenstat vlen124stat zipfstat \
#
BINS := $(patsubst %,%.bin,$(BINNAMES))

all : $(BINS)
replay.bin : replay.c replay.h lru.h lirs.h arc.h

%.bin : %.c %.h $(COMMONDEP)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm

%.bin : %.c $(COMMONDEP)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm

clean :
	rm -f *.o *~ $(BINS)

GCCINSTALL = "/usr/lib/gcc/$$(gcc -dumpmachine)/$$(gcc -dumpversion)"
check :
	cppcheck -I/usr/local/include -I/usr/include -I $(GCCINSTALL)/include -I $(GCCINSTALL)/include-fixed -DDUMMY --enable=all .
