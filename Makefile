.PHONY : all clean
COMMONH := libtrace3.h libtrace2.h libtrace.h Makefile
OPT := -ggdb -O0
#OPT := -O3
#OPT := -O3 -DNDEBUG

BINS =  statzipf genzipf stat4 arc lirs lrux lru spstat sampley samplex vlen print4 sort64 keymap conv allstat getstat printer types x2op4 vlengen statsp statvlen
all : $(BINS)

% : %.c %.h $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm
% : %.c $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4 -lm

clean :
	rm -f *.o *~ $(BINS)
