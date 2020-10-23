#include <stdio.h>
#include "c_map.h"

define_map(int, char)

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
