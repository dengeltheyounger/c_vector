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
	 memset_failed
} array_code; 

#define iterate_vector(VECTOR, ITER)	for (ITER; ITER < VECTOR->curr_index; ++ITER)
 
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
	array_code add_top_##DATA(struct c_vector_##DATA *vector, DATA value) {	\
		DATA* temp = NULL;	\
		DATA* memtemp = NULL;	\
		if ((vector->curr_index+1) > vector->current_size) {	\
			temp = (DATA*) realloc(vector->data, sizeof(DATA)*2*vector->max_size);	\
				\
			if (temp == NULL) {	\
				return realloc_failed;	\
			}	\
			vector->data = temp;	\
			memtemp = &vector->data[vector->max_size-1];	\
			bzero(memtemp, sizeof(DATA)*vector->max_size);	\
			if (temp == NULL) {	\
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
	static inline void remove_top_##DATA(c_vector_##DATA *vector) {	\
		if (vector->curr_index == 0) {	\
			return;	\
		}	\
		vector->data[vector->curr_index - 1] = 0;	\
		--(vector->curr_index);	\
	}	\
	static inline size_t get_current_index_##DATA(c_vector_##DATA *vector) {	\
		return vector->curr_index;	\
	}	\
		\
	static inline size_t get_current_size_##DATA(c_vector_##DATA *vector) {	\
		return vector->current_size;	\
	}	\
		\
	static inline size_t get_max_size_##DATA(c_vector_##DATA *vector) {	\
		return vector->max_size;	\
	}	\
		\
	static inline void set_vector_ptr_##DATA(c_vector_##DATA* vector) {	\
		vector->destroy_vector = &destroy_c_vector_##DATA;	\
		vector->add_top = &add_top_##DATA;	\
		vector->get_current_index = &get_current_index_##DATA; \
		vector->get_current_size = &get_current_size_##DATA; \
		vector->get_max_size = &get_max_size_##DATA; \
		vector->remove_top = &remove_top_##DATA;	\
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
			vector->max_size = 2;	\
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
	
#define c_vector(DATA) c_vector_##DATA
#define new_c_vector(DATA, NUMBER) new_vector_##DATA((size_t) NUMBER)
	
#endif
