.PHONY : all clean
COMMONH := libtrace3.h libtrace2.h libtrace.h Makefile
#OPT := -ggdb -O0
#OPT := -O3
OPT := -O3 -DNDEBUG

BINS =  arc lirs lrux lru mergesizes \
        op42spx op4print op4stat op4genzipf \
        orig2op4 origkeymap origprint origstat origstatget origstattype origvlen \
        spx2op4 spxgenzipf spxstat spxstatv \
        u64sort vlengen vlenstat zipfstat \
#

all : $(BINS)

% : %.c %.h $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm
% : %.c $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm

clean :
	rm -f *.o *~ $(BINS)
