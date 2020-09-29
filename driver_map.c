#include <stdio.h>
#include <stddef.h>
#include "c_map.h"

define_map(int, char)

int main() {
	c_map(int, char) *map = NULL;
	printf("Testing int to char map creation\n");
	map = new_c_map(int, char);
	
	if (map == NULL) {
		fprintf(stderr, "new_map returned NULL!\n");
		return 1;
	}
	
	printf("int to char map creation test successful\n");
	
	size_t msize = sizeof(c_map(int, char));
	printf("Size of c_map struct: %ld bytes\n", msize);
	
	printf("Testing map destruction\n");
	map = map->destroy_map(map);
	printf("map destruction test successful\n");
	
	return 0;
}
