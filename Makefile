.PHONY : all clean
COMMONDEP := libtrace3.h libtrace2.h libtrace.h Makefile
#OPT := -ggdb -O0
#OPT := -O3
OPT := -Wall -Wextra -march=native -O3 -DNDEBUG

BINNAMES := arc lirs lrux lru mergesizes \
    op42spx op4print op4stat op4genzipf \
    orig2op4 origkeymap origprint origstat origstatget origstattype origvlen origkvlen \
    spx2op4 spxgenzipf spxstat spxstatv \
    u64sort vlengen vlenstat zipfstat \
#
BINS := $(patsubst %,%.bin,$(BINNAMES))

all : $(BINS)

%.bin : %.c %.h $(COMMONDEP)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm
%.bin : %.c $(COMMONDEP)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm

clean :
	rm -f *.o *~ $(BINS)
