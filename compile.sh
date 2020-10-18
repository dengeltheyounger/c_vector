#!/bin/bash

if [[ -f "output" ]]; then
	rm output
fi

if [[ -f "expanded.c" ]]; then
	rm expanded.c
fi

gcc -E -P -C driver_rbtree.c >> output
sed -in "s://#:#:g" output
clang-format output >> expanded.c
gcc -o driver expanded.c -ggdb -Wall
