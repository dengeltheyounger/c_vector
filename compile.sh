#!/bin/bash

if [[ -f "rboutput" ]]; then
	rm rboutput*
fi

if [[ -f "coutput" ]]; then
	rm coutput*
fi

if [[ -f "rbexpand.c" ]]; then
	rm rbexpand.c
fi

if [[ -f "cmapexpand.c" ]]; then
	rm cmapexpand.c
fi

gcc -E -P -C driver_rbtree.c >> rboutput
gcc -E -P -C driver_cmap.c >> coutput
sed -in "s://#:#:g" rboutput
sed -in "s://#:#:g" coutput
clang-format rboutput >> rbexpand.c
clang-format coutput >> cmapexpand.c
gcc -o rbdriver rbexpand.c -ggdb -Wall
gcc -o cdriver cmapexpand.c -ggdb -Wall
