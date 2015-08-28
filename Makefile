all : allstat getstat printer types
% : %.c libtrace.h
	clang -std=gnu11 -O3 -o $@ $< -llz4
