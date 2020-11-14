#ifndef __cplusplus
#include <stdio.h>
#else
#include <cstdio>
#endif
#include "c_vector.h"
#include "error.h"


define_vector(int)

int main(int argc, char *argv[]) {
	printf("The size of data type: %d\n", sizeof(int));
	c_vector(int) *vector = NULL;
	size_t currindex = 0;
	size_t currsize = 0;
	size_t maxsize = 0;
	error_code code = success;
	
	printf("Testing int vector creation\n");
	vector = new_c_vector(int, 0);
	// Type casting is needed because new_vector_iterator returns a generic_iterator
	vector_iterator(int) *iter = (vector_iterator(int) *) new_vector_iterator(int, vector);
	generic_iterator *giter = (generic_iterator *) iter;
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("int vector creation successful\n");
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("Testing add_top function\n");
	
	code = vector->add_top(vector, 1);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	code = vector->add_top(vector, 2);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("add_top function test successful\n");
	
	printf("Testing resize function\n");

	code = vector->resize(vector, 24);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);

	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);

	code = vector->resize(vector, 36);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);


	printf("current index: %ld, current size: %ld, max size %ld\n", currindex, currsize, maxsize);	

	code = vector->resize(vector, 24);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);

	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("Resize function test successful\n");
	
	printf("Testing add_top after use of resize function\n");

	code = vector->add_top(vector, 3);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}	

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("add_top function test after resize function use successful\n");
	
	printf("Testing shrink function\n");
	
	code = vector->shrink(vector);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);

	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	code = vector->shrink(vector);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", err_struct.code);
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("shrink function test successful\n");
		
	for (giter; !giter->end(giter); giter->next(giter))
		printf("Value at next index is %d\n", iter->current(iter));
	
	printf("Iteration successful\n");
	
	printf("Testing remove_top function\n");
	
	vector->remove_top(vector);
	
	for (giter; !giter->end(giter); giter->next(giter))
		printf("Value at next index is %d\n", iter->current(iter));
	
	printf("remove_top test successful\n");
	
	printf("Testing insert function\n");
	vector->insert(vector, 1,  500);

	for (giter; !giter->end(giter); giter->next(giter))
		printf("Value at next index is %d\n", iter->current(iter));
	
	printf("insert function test successful\n");
		
	size_t vsize = sizeof(c_vector(int));
	size_t isize = sizeof(vector_iterator(int));

	printf("Size of vector is %ld bytes.\n", vsize);
	printf("Size of vector_iterator is %ld bytes.\n", isize);
	
	printf("Testing destroy_vector function\n");
	vector->destroy_vector(vector);
	giter = giter->destroy_iterator(giter);
	printf("destroy vector_function test successful\n");
	return 0;
}
