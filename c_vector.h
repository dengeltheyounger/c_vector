#ifndef C_VECTOR_H
#define C_VECTOR_H
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
// This is a convenient shorthand for typechecking
#define check_types(A, B)	__builtin_types_compatible_p(typeof(A), typeof(B))

// This can be used to get type information for a c_vector
#define type_name(DATA_TYPE)	#DATA_TYPE
/* 
 * The name has been changed from array to c_vector
 * The following method allows the added advantage of compile-time typechecking
 * and also allows for class-like function pointers
 */
 
 typedef enum array_code {
	 // This allows 0 as a success code
	 realloc_failed = 1,
	 memset_failed,
	 invalid_index
} array_code; 

/* iterate_vector(VECTOR, ITER)
 * INPUT: VECTOR -> c_vector struct pointer, ITER -> size_t variable that points to starting index
 * OUTPUT: None
 * USAGE: iterate_vector(VECTOR, ITER) { code for iteration here... }
 * NOTES: This can be used for many different iteration operations. Just add brackets.
 */
#define iterate_vector(VECTOR, ITER)	for (ITER; ITER < VECTOR->curr_index; ++ITER)

/* code_to_string(CODE)
 * INPUT: CODE -> array_code value
 * OUTPUT: stringified array_code token
 * USAGE: char *error_value = code_to_string(error_code)
 * NOTES: This can be useful if the user desires to implement a logger for this array
 */ 

#define code_to_string(CODE) ({ #CODE; })
 
/* define_vector(DATA)
 * INPUT: DATA -> data type for desired vector
 * OUTPUT: None
 * USAGE: define_vector(int)
 * NOTES: This must be called before the vector and associated functions are used. 
 * define_vector essentially creates a set a c_vector struct and a set of functions
 * for the given data type. Using the overall approach allows the generality and ease
 * of programming that comes with macros, along with the type safety and space efficiency
 * of functions (since you don't have the same code copied and pasted into main 100 times).
 * Another option would be to use void * and then have macro wrappers that deal with the
 * data types. However, I think that that would be more error prone and more difficult to 
 * program.
 * 
 * c_vector_##DATA *destroy_c_vector_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA* -> c_vector struct pointer of a given data type
 * OUTPUT: None
 * USAGE: vector = vector->destroy_c_vector(vector);
 * NOTES: Destructor for the c_vector
 * 
 * array_code add_top_##DATA(c_vector_##DATA *vector, DATA value)
 * INPUT: c_vector_##DATA* -> c_vector struct pointer, DATA -> value of given data type
 * OUTPUT: enum array_code to indicate error or success
 * USAGE: array_code code = vector->add_top(vector, value);
 * NOTES: This function is meant to mirror the vector's push_back method. If
 * new memory needs to be allocated, this function will zero the newly allocated
 * memory.
 * 
 * void remove_top_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: none
 * USAGE: vector->remove_top(vector);
 * NOTES: This function zeros the last value added into array and then decrements curr_index.
 *
 * size_t get_current_index_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: value of c_vector's curr_index member
 * USAGE: size_t value = vector->get_current_index(vector);
 * NOTES: The current index is the next index available for the add_top function.
 * 
 * size_t get_current_size_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: current size of *data member
 * USAGE: size_t value = vector->get_current_size(vector);
 * NOTES: 
 *
 * size_t get_max_size_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: max size of *data member
 * USAGE: size_t value = vector->get_max_size(vector);
 * NOTES: 
 *
 * array_code insert_##DATA(c_vector_##DATA *vector, size_t index, DATA data)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * index -> index at which to insert
 * data -> value to insert at index
 * OUTPUT: 0 for success or invalid_index on failure
 * USAGE: array_code code = vector->insert(vector, index, data);
 * NOTES: 
 * 
 * DATA value_at(c_vector_##DATA *vector, size_t index)
 * INPUT c_vector_##DATA -> c_vector struct pointer
 * index -> index to retrieve value at
 * OUTPUT: value for success or 0 if index is invalid
 * NOTES: An alternative method for indicating error should be sought
 * 
 * array_code resize(c_vector_##DATA *vector, size_t num)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * num -> number of elements (not number of bytes
 * OUTPUT: 0 for success or array_code for failure
 * NOTES: memset is used in the case max size is increased or curr_index
 * is shrunk.
 * 
 * void set_vector_ptr_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: None
 * USAGE: Internal use only
 * NOTES: This sets the function pointers for c_vector struct when new_vector is called
 *  
 * c_vector_##DATA *new_vector_##DATA(size_t number) 
 * INPUT: size_t number -> number of elements to allocate
 * OUTPUT: c_vector struct pointer
 * USAGE: internal use only, use the macro wrapper defined above
 * NOTES: This creates a new vector of a given data type. If the number of elements to
 * allocate is 0, then the function will allocate space for 2 elements. Any memory pre-allocated
 * is zeroed before the struct pointer is returned to the caller
 */
#define define_vector(DATA)	\
	typedef struct c_vector_##DATA { \
		size_t max_size;	\
		size_t current_size; \
		size_t curr_index; \
		DATA *data; \
		char *data_type; \
		struct c_vector_##DATA *(*destroy_vector)(struct c_vector_##DATA*);	\
		array_code (*add_top)(struct c_vector_##DATA*, DATA value);	\
		void (*remove_top)(struct c_vector_##DATA*);	\
		size_t (*get_current_index)(struct c_vector_##DATA*);	\
		size_t (*get_current_size)(struct c_vector_##DATA*);	\
		size_t (*get_max_size)(struct c_vector_##DATA*);	\
		array_code (*insert)(struct c_vector_##DATA*, size_t, DATA);	\
		DATA (*value_at)(struct c_vector_##DATA*, size_t);	\
		array_code (*resize)(struct c_vector_##DATA*, size_t);	\
	} c_vector_##DATA;	\
						\
	c_vector_##DATA *destroy_c_vector_##DATA(c_vector_##DATA* vector) {	\
		if (vector == NULL) {	\
			return NULL;	\
		}	\
			\
		if (vector->data != NULL) {	\
			free(vector->data);	\
		}	\
		free(vector);	\
		return NULL;	\
	}					\
						\
	array_code add_top_##DATA(c_vector_##DATA *vector, DATA value) {	\
		DATA* temp = NULL;	\
		DATA* memtemp = NULL;	\
		if ((vector->curr_index+1) > vector->current_size) {	\
			temp = (DATA*) realloc(vector->data, sizeof(DATA)*2*vector->max_size);	\
				\
			if (temp == NULL) {	\
				return realloc_failed;	\
			}	\
			vector->data = temp;	\
			size_t index = (vector->max_size / sizeof(DATA)) - 1;	\
			memtemp = (DATA*) memset(&(vector->data[index]), 0, sizeof(DATA)*vector->max_size);	\
			if (memtemp == NULL) {	\
				return memset_failed;	\
			}	\
			temp = NULL;	\
			vector->data[vector->curr_index] = value;	\
			vector->current_size = vector->max_size;	\
			vector->max_size *= 2;	\
			++(vector->curr_index);	\
			return 0;	\
		}	\
			\
		vector->data[vector->curr_index] = value;	\
		++vector->curr_index;	\
		return 0;	\
	}	\
		\
	void remove_top_##DATA(c_vector_##DATA *vector) {	\
		if (vector->curr_index == 0) {	\
			return;	\
		}	\
		vector->data[vector->curr_index - 1] = 0;	\
		--(vector->curr_index);	\
	}	\
	size_t get_current_index_##DATA(c_vector_##DATA *vector) {	\
		return vector->curr_index;	\
	}	\
		\
	size_t get_current_size_##DATA(c_vector_##DATA *vector) {	\
		return (vector->current_size / sizeof(DATA));	\
	}	\
		\
	size_t get_max_size_##DATA(c_vector_##DATA *vector) {	\
		return (vector->max_size / sizeof(DATA));	\
	}	\
		\
		\
	array_code insert_##DATA(c_vector_##DATA *vector, size_t index, DATA value) {	\
		if (index > (vector->current_size - 1)) {	\
			return invalid_index;	\
		}	\
			\
		vector->data[index] = value;	\
			\
		if (index > vector->curr_index) {	\
			vector->curr_index = index;	\
		}	\
			\
		return 0;	\
		\
	}	\
		\
	DATA value_at_##DATA(c_vector_##DATA *vector, size_t index) {	\
		if (index >= vector->curr_index) {	\
			return 0;	\
		}	\
			\
		return vector->data[index];	\
	}	\
		\
	array_code resize_##DATA(c_vector_##DATA *vector, size_t elementnum) {	\
		size_t newsize = elementnum*sizeof(DATA);	\
		DATA *temp = NULL;	\
		DATA *memtemp = NULL;	\
		if (newsize == vector->current_size) {	\
			return 0;	\
		}	\
		else if (newsize > vector->current_size && newsize < vector->max_size) {	\
			vector->current_size = newsize;	\
			return 0;	\
		}	\
			\
		else if (newsize < vector->current_size) {	\
			size_t index = elementnum - 1;	\
			temp = (DATA*) memset(&(vector->data[index]), 0, (vector->max_size - newsize));	\
			if (temp == NULL) {	\
				return memset_failed;	\
			}	\
			vector->current_size = newsize;	\
			if (vector->curr_index > index) {	\
				vector->curr_index = index;	\
			}	\
				\
			return 0;	\
		}	\
			\
		temp = (DATA*) realloc(vector->data, 2*newsize);	\
		if (temp == NULL) {	\
			return realloc_failed;	\
		}	\
		vector->data = temp;	\
		temp = NULL;	\
		size_t index = (vector->max_size / sizeof(DATA)) - 1;	\
		memtemp = (DATA*) memset(&(vector->data[index]), 0, ((2*newsize) - vector->max_size));	\
		if (memtemp == NULL) {	\
			return memset_failed;	\
		}	\
		vector->current_size = newsize;	\
		vector->max_size = 2*newsize;	\
		return 0;	\
	}	\
	void set_vector_ptr_##DATA(c_vector_##DATA* vector) {	\
		vector->destroy_vector = &destroy_c_vector_##DATA;	\
		vector->add_top = &add_top_##DATA;	\
		vector->get_current_index = &get_current_index_##DATA; \
		vector->get_current_size = &get_current_size_##DATA; \
		vector->get_max_size = &get_max_size_##DATA; \
		vector->remove_top = &remove_top_##DATA;	\
		vector->insert = &insert_##DATA;	\
		vector->value_at = &value_at_##DATA;	\
		vector->resize = &resize_##DATA;	\
	}	\
	\
	c_vector_##DATA *new_vector_##DATA(size_t number) {	\
		c_vector(DATA) *vector = NULL;	\
										\
		vector = (c_vector(DATA) *) calloc(1, sizeof(c_vector(DATA)));	\
				\
		if (vector == NULL) {	\
			return NULL;	\
		}	\
			\
		if (number == 0) {	\
			vector->data = (DATA*) calloc(2, sizeof(DATA));	\
				\
			if (vector->data == NULL) {	\
				free(vector);	\
				return NULL;	\
			}	\
				\
			vector->max_size = 2*sizeof(DATA);	\
			vector->current_size = 0;	\
		}	\
			\
		else {	\
			vector->data = (DATA*) calloc(2*number, sizeof(DATA));	\
				\
			if (vector->data == NULL) {	\
				free(vector);	\
				return NULL;	\
			}	\
				\
			vector->max_size = 2*number;	\
			vector->current_size = number;	\
		}	\
		vector->curr_index = 0;	\
		vector->data_type = type_name(DATA);	\
		set_vector_ptr_##DATA(vector);	\
		return vector;	\
	}	
	
/* c_vector(DATA)
 * INPUT: DATA -> the data type desired for the array
 * OUTPUT: None
 * USAGE: c_vector(int) *data = NULL;
 * NOTES: This is used a wrapper and abstracts away some of the internal naming conventions above.
 * If the user wishes to reference a c_vector of type double, they can refer to it as 
 * c_vector(double) instead of c_vector_double.
 * Make sure to typedef any multi-word data types. The c_vector can only handle single word data types.
 */
#define c_vector(DATA) c_vector_##DATA
/* new_c_vector(DATA, NUMBER)
 * INPUT: DATA -> the data type desired for the array, NUMBER -> the number of elements to preallocate
 * OUTPUT: Pointer to newly created c_vector struct
 * USAGE: c_vector(int) *vector = new_c_vector(int, 0);
 * NOTES: new_vector_##DATA does not make sense as a function pointer for the c_vector struct. new_vector
 * is in charge of creating the vector struct, and would lead to a catch 22. For that reason, this
 * wrapper was created in order to abstract away the naming convention above. Instead of worrying about
 * how exactly how the data type is used to uniquely name the function, one need only specify the 
 * data type and number of elements.
 */
#define new_c_vector(DATA, NUMBER) new_vector_##DATA((size_t) NUMBER)
	
#endif
