#include <stdio.h>
#include "c_vector.h"

// driver code for the c_vector

#define STRINGIFY(CODE)	#CODE

define_vector(int)

void print_byte(void *ptr, size_t size) {
	unsigned char *temp = ptr;
	
	printf("\nByte value of struct: \n");
	
	while (size--)
		printf("%02x", *temp--);
		
	printf("\n");
}

int main(int argc, char *argv[]) {
	printf("The size of data type: %d\n", sizeof(int));
	c_vector(int) *vector = NULL;
	size_t currindex = 0;
	size_t currsize = 0;
	size_t maxsize = 0;
	size_t iter = 0;
	array_code code = 0;
	
	printf("Testing int vector creation\n");
	vector = new_c_vector(int, 0);
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("int vector creation successful\n");
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("Testing add_top function\n");
	
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
	
	printf("add_top function test successful\n");
	
	printf("Testing resize function\n");

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
	
	printf("Resize function test successful\n");
	
	printf("Testing add_top after use of resize function\n");

	code = vector->add_top(vector, 3);

	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}	

	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("add_top function test after resize function use successful\n");
	
	printf("Testing shrink function\n");
	
	code = vector->shrink(vector);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);

	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	code = vector->shrink(vector);
	
	if (code != 0) {
		fprintf(stderr, "Value of code: %s", STRINGIFY(code));
	}
	
	currindex = vector->get_current_index(vector);
	currsize = vector->get_current_size(vector);
	maxsize = vector->get_max_size(vector);
	
	printf("current index: %ld, current size: %ld, max size: %ld\n", currindex, currsize, maxsize);
	
	printf("shrink function test successful\n");
	
	printf("Iterating vector\n");
	
	iterate_vector(vector, iter) {
		printf("Value at index %ld: %d\n", iter, vector->value_at(vector, iter));
	}
	
	printf("Iteration successful\n");
	
	printf("Testing remove_top function\n");
	
	vector->remove_top(vector);
	iter = 0;
	
	iterate_vector(vector, iter) {
		printf("Value at index %ld: %d\n", iter, vector->value_at(vector, iter));
	}
	
	printf("remove_top test successful\n");

	iter = 0;
	
	printf("Testing insert function\n");
	vector->insert(vector, 1,  500);

	iterate_vector(vector, iter) {
		printf("Value at index %ld: %d\n", iter, vector->value_at(vector, iter));
	}
	
	printf("insert function test successful\n");
	
	print_byte(vector, sizeof(c_vector(int)));
	
	size_t vsize = sizeof(c_vector(int));

	printf("Size of vector is %ld bytes.\n", vsize);
	
	printf("Testing destroy_vector function\n");
	vector->destroy_vector(vector);
	printf("destroy vector_function test successful\n");
	
	return 0;
}
