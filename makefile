driver_vector: driver.c 
	gcc -o driver_vector driver.c

driver_rbtree: driver_rbtree.c
	gcc -o driver_rbtree driver_rbtree.c

driver_cmap: driver_cmap.c
	gcc -o driver_cmap driver_cmap.c

all: driver.c driver_rbtree.c driver_cmap.c
	gcc -o driver_vector driver.c
	gcc -o driver_rbtree driver_rbtree.c
	gcc -o driver_cmap driver_cmap.c

clean:
	@if [ -f driver_vector ]; then rm driver_vector; fi					
	@if [ -f driver_cmap ]; then rm driver_cmap; fi
	@if [ -f driver_rbtree ]; then rm driver_rbtree; fi
