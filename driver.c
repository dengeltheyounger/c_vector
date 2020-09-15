#include <stdio.h>
#include "array_utilities2.h"

define_array(int);

int main(int argc, char *argv[]) {
	int error = 0;
	size_t iter = 0;
	array(int) *arr = NULL;
	
	arr = new_array(arr, int, 0, error);
	
	printf("current index: %zu, current size: %zu, max size: %zu\n", get_current_index(arr), get_current_size(arr), get_max_size(arr));
	
	arr = push(arr, 1, error);
	
	printf("current index: %zu, current size: %zu, max size: %zu\n", get_current_index(arr), get_current_size(arr), get_max_size(arr));
	
	arr = push(arr, 95, error);
	
	printf("current index: %zu, current size: %zu, max size: %zu\n", get_current_index(arr), get_current_size(arr), get_max_size(arr));
		
	arr = push(arr, 1028, error);
	
	printf("current index: %zu, current size: %zu, max size: %zu\n", get_current_index(arr), get_current_size(arr), get_max_size(arr));
	
	iterate(arr, iter) {
		printf("Value at index %zu: %d\n", iter, value_at(arr, iter, error));
	} 
		
	arr = delete_array(arr);
	
	return 0;
}
