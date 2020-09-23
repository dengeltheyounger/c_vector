#include <stdio.h>
#include "c_vector.h"

// driver code for the c_vector

#define STRINGIFY(CODE)	#CODE

define_vector(int)

int main(int argc, char *argv[]) {
	printf("The size of data type: %ld\n", sizeof(int));
	c_vector(int) *vector = NULL;
	size_t currindex = 0;
	size_t currsize = 0;
	size_t maxsize = 0;
	size_t iter = 0;
	array_code code = 0;
	
	vector = new_c_vector(int, 0);
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	code = vector->add_top(vector, 1);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	code = vector->add_top(vector, 2);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);

	code = vector->resize(vector, 24);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);

	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);

	code = vector->resize(vector, 36);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);


	printf("current index: %ld, current size: %ld, max size %ld\n", currindex, currsize, maxsize);	

	code = vector->resize(vector, 24);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);

	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);

	code = vector->add_top(vector, 3);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}	

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	iterate_vector(vector, iter) {
		printf("Value at index %ld: %d\n", iter, vector->value_at(vector, iter));
	}
	
	vector->remove_top(vector);
	iter = 0;
	
	iterate_vector(vector, iter) {
		printf("Value at index %ld: %d\n", iter, vector->value_at(vector, iter));
	}

	iter = 0;
	vector->insert(vector, 1,  500);

	iterate_vector(vector, iter) {
		printf("Value at index %ld: %d\n", iter, vector->value_at(vector, iter));
	}
	
	size_t vsize = sizeof(c_vector(int));

	printf("Size of vector is %ld bytes.\n", vsize);
	
	vector->destroy_vector(vector);
	return 0;
}
