#!/bin/bash

if [[ -f "rboutput" ]]; then
	rm rboutput
fi

if [[ -f "coutput" ]]; then
	rm coutput
fi

if [[ -f "soutput" ]]; then
	rm soutput
fi

if [[ -f "rbexpand.c" ]]; then
	rm rbexpand.c
fi

if [[ -f "cmapexpand.c" ]]; then
	rm cmapexpand.c
fi

if [[ -f "sexpand.c" ]]; then
	rm sexpand.c
fi

gcc -E -P -C driver_rbtree.c >> rboutput
gcc -E -P -C driver_cmap.c >> coutput
gcc -E -P -C stream_handler.c >> soutput
sed -i "s://#:#:g" rboutput
sed -i "s://#:#:g" coutput
sed -i "s://#:#:g" soutput
clang-format rboutput >> rbexpand.c
clang-format coutput >> cmapexpand.c
clang-format soutput >> sexpand.c
gcc -o rbdriver rbexpand.c sexpand.c -ggdb -Wall
gcc -o cdriver cmapexpand.c sexpand.c -ggdb -Wall
