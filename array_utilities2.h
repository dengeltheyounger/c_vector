#ifndef ARRAY_UTILITES_H
#define ARRAY_UTILITIES_H
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
// This is a convenient shorthand for typechecking
#define check_types(A, B)	__builtin_types_compatible_p(typeof(A), typeof(B))

typedef enum array_code {
	success,
	null_array,
	array_failed_malloc,
	data_failed_malloc,
	data_failed_realloc,
	index_out_of_range,
	parameter_type_mismatch,
	data_failed_memset
} array_code;

// curr_index is used for push function
#define define_array(DATA)		 \
	struct DATA##_array {		\
		size_t max_size;		\
		size_t current_size;	\
		size_t curr_index;		\
		DATA *data;				\
	}							\

#define array(DATA) struct DATA##_array

/* allocate array on the heap. Free arr if needed
 * If allocation fails, set error code exit loop to return null
 * If element number greater than zero, then calloc so many DATA elements 
 * Otherwise, calloc enough memory for 2 DATA elements
 * If data is null, free array, set error code, and exit loop to return null
 * set max and current size 
 */

/* Note that you should set arr to null before passing it in. */
#define new_array(arr, DATA_TYPE, ELEMENT_NUMBER, error_code) ({					\
	int _err = 1;																	\
																					\
	if (arr != NULL) {																\
		if (arr->data != NULL) {													\
			free(arr->data); 														\
		}																			\
		free(arr);																	\
	}																				\
																					\
	do {																			\
		if (!check_types(ELEMENT_NUMBER, size_t) && !check_types(ELEMENT_NUMBER, int)) { \
			error_code = parameter_type_mismatch;									\
			break;																	\
		}																			\
		if (!check_types(error_code, int) && !check_types(error_code, array_code)) { \
			error_code = parameter_type_mismatch;									\
			break;																	\
		}																			\
		arr = (array(DATA_TYPE)*) malloc(sizeof(array(DATA_TYPE)));					\
		if (arr == NULL) {															\
			error_code = array_failed_malloc;										\
			break;																	\
		}																			\
																					\
		if (ELEMENT_NUMBER > 0) {													\
			arr->data = (DATA_TYPE*) calloc(ELEMENT_NUMBER*2, sizeof(DATA_TYPE));	\
			arr->max_size = ELEMENT_NUMBER*2;										\
			arr->current_size = ELEMENT_NUMBER;										\
		}																			\
		else {																		\
			arr->data = (DATA_TYPE*) calloc(2, sizeof(DATA_TYPE));					\
			arr->max_size = 2;														\
			arr->current_size = 0;													\
		}																			\
																					\
		if (arr->data == NULL) {													\
			free(arr);																\
			error_code = data_failed_malloc;										\
			break;																	\
		}																			\
		arr->curr_index = 0;														\
																					\
		_err = 0;																	\
	} while (0);																	\
																					\
																					\
	if (_err == 1)																	\
		arr = NULL;																	\
	arr;																			\
})	

/* if the provided array is null, break and return null
 * if data is not already null, free it
 * free the array and set to null
 * return null
 */
 
#define delete_array(ARRAY) ({														\
	do {																			\
		if (ARRAY != NULL) {														\
			if (ARRAY->data != NULL) {												\
				free(ARRAY->data);													\
			}																		\
			free(ARRAY); ARRAY = NULL;												\
		}																			\
	} while (0);																	\
																					\
	ARRAY;																			\
})

/* represent data with a temporary array
 * get current position and max position
 * if current position + 1 will be greater than or equal to current size
 * reallocate twice max size
 * check to make sure that allocation worked
 * copy data into temp
 * point data to temp
 * set array current position + 1 equal to DATA
 * increment current position, set current size to max size and double max size
 * else if current position + 1 is less than current size
 * set data at current + 1 equal to DATA
 * return ARRAY
 */
#define push(ARRAY, DATA, error_code) ({											\
	size_t current_pos = ARRAY->curr_index;											\
	typeof(ARRAY->data) temp = NULL;												\
	typeof(ARRAY->data) temp2 = NULL;												\
																					\
	do {																			\
																					\
		if (!check_types(typeof(ARRAY->data[0]), typeof(DATA))) {					\
			error_code = parameter_type_mismatch;									\
			break;																	\
		}																			\
																					\
		if (current_pos >= ARRAY->current_size) {									\
			temp = (typeof(ARRAY->data)) realloc(ARRAY->data, sizeof(ARRAY->data[0])*ARRAY->max_size*2);	\
			if (temp == NULL) {														\
				error_code = data_failed_realloc;									\
				break;																\
			}																		\
																					\
			/* realloc does not intialize. Initialize here. */						\
			temp2 = (typeof(ARRAY->data)) memset(temp+ARRAY->max_size, 0, sizeof(ARRAY->data[0])*ARRAY->max_size); \
			/* We don't care about temp2. Just checking result of memset */			\
																					\
			if (temp2 == NULL) 														\
				error_code = data_failed_memset;									\
																					\
			ARRAY->data = temp;														\
			ARRAY->data[current_pos] = DATA;										\
			ARRAY->curr_index += 1;													\
			ARRAY->current_size = ARRAY->max_size;									\
			ARRAY->max_size *= 2;													\
		}																			\
																					\
		else {																		\
			ARRAY->data[current_pos] = DATA;										\
			ARRAY->curr_index += 1;													\
		}																			\
																					\
	} while (0);																	\
																					\
	ARRAY;																			\
})

/* This just iterates through the array. The user can decide what to do in the loop 
 * The user passes a size_t variable. If the user wishes to iterate through the whole
 * array, set variable to 0. Otherwise, set at start index
 */

#define iterate(ARRAY, ITER) for(ITER; ITER < ARRAY->curr_index; ++ITER) 
#define get_max_size(ARRAY) ({ (ARRAY != NULL) ? ARRAY->max_size : 0; })
#define get_current_size(ARRAY) ({ (ARRAY != NULL) ? ARRAY->current_size : 0; })
#define get_current_index(ARRAY) ({ (ARRAY != NULL) ? ARRAY->curr_index : 0; })
/* get the value at index. will return 0 if index > current_size - 1 */
// Each parameter is checked before proceeding with the do while loop
#define value_at(ARRAY, INDEX, error_code) ({										\
																					\
	int _err = 1;																	\
	typeof(ARRAY->data[0]) value = 0;												\
																					\
	do {																			\
		if (!check_types(INDEX, int) && !check_types(INDEX, size_t)) {	\
			error_code = parameter_type_mismatch;									\
			break;																	\
		}																			\
																					\
		if (!check_types(error_code, int) && !check_types(error_code, array_code)) {	\
			error_code = parameter_type_mismatch;									\
			break;																	\
		}																			\
																					\
		if (INDEX > (ARRAY->current_size-1)) {										\
			error_code = index_out_of_range;										\
			break;																	\
		}																			\
																					\
		_err = 0;																	\
	} while(0);																		\
																					\
	if (_err == 0) {																\
		value = ARRAY->data[INDEX];													\
	}																				\
																					\
	value;																			\
})	

#define find(ARRAY, VALUE, error_code) ({											\
	

#endif

//https://codereview.stackexchange.com/questions/235294/generic-dynamic-array-implementation-in-c
