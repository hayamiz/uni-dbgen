
AM_CFLAGS = -g -Wall -msse -msse2
LDFLAGS = -no-undefined
INCLUDES =

noinst_LTLIBRARIES =							\
	libuni-dbgen.la

bin_PROGRAMS = uni-dbgen

uni_dbgen_headers =				\
	uni-dbgen.h


libuni_dbgen_la_SOURCES = uni-dbgen.c

uni_dbgen_SOURCES = uni-dbgen-main.c $(uni_dbgen_headers)
uni_dbgen_LDADD = libuni-dbgen.la
uni_dbgen_LDFLAGS = \
	-pthread
