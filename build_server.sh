#!/bin/sh

#
# SSL/TLS Test
#  - hjkim, 2019.03.07
#



GCC_BIN=/usr/bin/g++


INCS=""
LIBS="-lssl -lcrypto"
CFLAGS=""
OPTS="-Wall -O2 -g"

OUTPUT="server"
SRC="server.cpp"

echo $GCC_BIN -o $OUTPUT $SRC $CFLAGS $OPTS $INCS $LIBS
$GCC_BIN -o $OUTPUT $SRC $CFLAGS $OPTS $INCS $LIBS
