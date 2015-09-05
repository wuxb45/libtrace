COMMONH := libtrace3.h libtrace2.h libtrace.h Makefile
OPT := -O3

all : sampley samplex lru vlen print4 sort64 keymap conv allstat getstat printer types

% : %.c %.h $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4
% : %.c $(COMMONH)
	clang -std=gnu11 $(OPT) -o $@ $< -llz4
