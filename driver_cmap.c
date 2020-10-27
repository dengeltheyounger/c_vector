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
	int keyset[50];
	int chosen[50];
	size_t keynum;
	size_t nextindex;
} key_holder;

int main(void) {
	key_holder keyset;
	int key, iterkey;
	char val, iterval;
	map_code result;
	size_t count = 0;
	
	fprintf(stderr, "Testing constructor\n");
	
	c_map(int, char) *map = new_c_map(int, char);
	
	if (map == NULL) {
		fprintf(stderr, "Map creation failed!\n");
		return 1;
	}
	
	fprintf(stderr, "Address of map pointed to: %p\n", (void *) map);
	fprintf(stderr, "Address of tree in map: %p\n", (void *) map->tree);
	
	/* Note that the data types are provided as the first two arguments
	 * and the third type is a map. Imagine int and char being part of the
	 * template and map being part of the parameterized constructor
	 */
	generic_iterator *giter = new_map_iterator(int, char, map);
	map_iterator(int, char) *iter = (map_iterator(int, char) *) giter;
	
	if (giter == NULL) {
		fprintf(stderr, "Map iterator creation failed\n");
		return 1;
	}
	
	fprintf(stderr, "Constructor testing successful\n\n");
	
	fprintf(stderr, "Testing insertion and iteration\n");
	
	for (size_t i = 0; i < 50; ++i) {
		key = get_key();
		val = get_val();
		result = map->insert(map, key, val);
		if (result != 0) {
			fprintf(stderr, "Value of result: %d\n", result);
			return 1;
		}
		if (!map->is_key(map, key)) {
			fprintf(stderr, "Insertion failed!\n");
			return 1;
		}
	}
		
	for (giter->first(giter); !giter->end(giter); giter->next(giter), ++count) {
		iterkey = iter->key;
		iterval = iter->value;
		fprintf(stderr, "Key %d, value %d\n", iterkey, iterval);
	}
	
	fprintf(stderr, "Number of keys iterated: %ld\n", count);
	fprintf(stderr, "Insertion and iteration successful\n");
	
	fprintf(stderr, "Testing destructor\n");
	
	map = map->destroy_map(map);
	giter = giter->destroy_iterator(giter); iter = NULL;
	
	fprintf(stderr, "Destructor testing successful\n\n");
	
	fprintf(stderr, "Size of c_map: %ld bytes\n", sizeof(c_map(int,char)));
	fprintf(stderr, "Size of map_iterator %ld bytes\n", sizeof(map_iterator(int, char)));
	
	return 0;
}
