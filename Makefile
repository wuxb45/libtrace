all : print4 sort64 keymap conv allstat getstat printer types
% : %.c libtrace.h Makefile
	clang -std=gnu11 -O3 -o $@ $< -llz4
