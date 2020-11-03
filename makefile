driver_vector: driver.c
	gcc -o driver_vector driver.c -ggdb

driver_rbtree: driver_rbtree.c
	gcc -o driver_rbtree driver_rbtree.c -ggdb

driver_cmap: driver_cmap.c
	gcc -o driver_cmap driver_cmap.c -ggdb

all: driver.c driver_rbtree.c driver_cmap.c
	gcc -o driver_vector driver.c -ggdb
	gcc -o driver_rbtree driver_rbtree.c -ggdb
	gcc -o driver_cmap driver_cmap.c -ggdb

clean:
	@if [ -f driver_vector ]; then rm driver_vector; fi					
	@if [ -f driver_cmap ]; then rm driver_cmap; fi
	@if [ -f driver_rbtree ]; then rm driver_rbtree; fi
