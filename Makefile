.PHONY : all clean
COMMONH := libtrace3.h libtrace2.h libtrace.h Makefile
OPT := -O3
#OPT := -O3 -NDEBUG
#OPT := -ggdb

BINS = lirs sampley samplex lru vlen print4 sort64 keymap conv allstat getstat printer types
all : $(BINS)

% : %.c %.h $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4
% : %.c $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4

clean :
	rm -f *.o *~ $(BINS)
