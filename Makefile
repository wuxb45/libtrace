COMMONH := libtrace2.h libtrace.h Makefile
all : samplex lru vlen print4 sort64 keymap conv allstat getstat printer types

% : %.c %.h $(COMMONH)
	clang -std=gnu11 -O3 -o $@ $< -llz4
% : %.c $(COMMONH)
	clang -std=gnu11 -O3 -o $@ $< -llz4
