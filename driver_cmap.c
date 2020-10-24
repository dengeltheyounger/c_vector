#ifndef __cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#else
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstddef>
#endif
#include "c_map.h"

define_map(int, char)

#define get_key()	({ int x = rand() % 100000; x; })
#define get_val()	({ int x = (rand() % 10) + 48; x; })

typedef struct key_holder {
	int keyset[10];
	int chosen[10];
	size_t keynum;
	size_t nextindex;
} key_holder;

// This is inefficient, but it'll work
int choose_key(key_holder *holder) {
	size_t index = 0;
	int foundindex = 0;
	// Get a random index and check to see if that index has already been used
	while (1) {
		foundindex = 0;
		index = (rand() % holder->keynum);
		// If it has, then repeat process
		for (size_t i = 0; i < holder->nextindex; ++i) {
			if (index == holder->chosen[i]) {
				foundindex = 1;
				break;
			}
		}
		/* If it hasn't, then add the new index to the list of chosen indices
		 * increment nextindex for the next time
		 * return the key
		 */
		if (!foundindex) {
			holder->chosen[holder->nextindex] = index;
			++holder->nextindex;
			return holder->keyset[index];
		}
	}
}

int main(void) {
	
	fprintf(stderr, "Testing constructor\n");
	
	c_map(int, char) *map = new_c_map(int, char);
		
	if (map == NULL) {
		fprintf(stderr, "Failed to allocate map!\n");
		return 1;
	}
	
	fprintf(stderr, "Constructor testing successful\n\n");
	
	fprintf(stderr, "Testing destructor\n");
	
	map = map->destroy_map(map);
	
	fprintf(stderr, "Destructor testing successful\n\n");
	
	fprintf(stderr, "Size of c_map: %ld bytes\n", sizeof(c_map(int,char)));
	
	return 0;
}
