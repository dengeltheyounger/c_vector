#ifndef	ERROR_H
#define	ERROR_H
#ifndef __cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <cstdio>
#include <cstdlib>
#include <cstring>
#endif

#define TO_STRING(ENUM) #ENUM

typedef struct error_info {	
	char *filename;	
	char *functname;
	char *line;	
	char *code;	
} error_info;	

error_info err_struct;

// This will be shared by c_vector, c_map, and red_and_black_tree
typedef enum error_code {
	success,
	realloc_failed,
	memset_failed,
	invalid_index,
	make_sentinels_failed,
	new_node_failed,
	basic_insert_failed,
	memcpy_failed,
	key_not_found,
	null_tree
} error_code;

static const char *error_code_string[] = {
	TO_STRING(success),
	TO_STRING(realloc_failed),
	TO_STRING(memset_failed),
	TO_STRING(invalid_index),
	TO_STRING(make_sentinels_failed),
	TO_STRING(new_node_failed),
	TO_STRING(basic_insert_failed),
	TO_STRING(memcpy_failed),
	TO_STRING(key_not_found),
	TO_STRING(null_tree)
};

error_code err;

/* The reason why I use this macro is because I intend to use the
 * __attribute__ macro in order to have it deallocate memory as soon 
 * this goes out of scope (in other words, before the program ends).
 */
#ifndef __GNUC__
	// If we're not using gcc, the programmer will need to manually deallocate
	static inline void free_error_info() {
		if (err_struct.filename != NULL) {
			free(err_struct.filename);
			err_struct.filename = NULL;
		}
		if (err_struct.functname != NULL) {
			free(err_struct.functname);
			err_struct.functname = NULL;
		}
		if (err_struct.line != NULL) {
			free(err_struct.line);
			err_struct.line = NULL;
		}
		if (err_struct.code != NULL) {
			free(err_struct.code);
			err_struct.code = NULL;
		}
	}
#else
	__attribute__((destructor)) static inline void free_error_info() {
		if (err_struct.filename != NULL) {
			free(err_struct.filename); 
			err_struct.filename = NULL;
		}
		if (err_struct.functname != NULL) {
			free(err_struct.functname); 
			err_struct.functname = NULL;
		}
		if (err_struct.line != NULL) {
			free(err_struct.line);
			err_struct.line = NULL;
		}
		if (err_struct.code != NULL) {
			free(err_struct.code);
			err_struct.code = NULL;
		}
	}
#endif

static inline void set_error_info(char *filename, char *functname, int line) {
	free_error_info();
	/* This makes error.c dependent on C99 standard. However, it seems
	 * to be the best way to convert an integer to a string (short of
	 * writing a custom function).
	 */
	 // Get array size based on digits in line number
	size_t arrsize = snprintf(NULL, 0, "%d", line);
	// Create array and add another byte for '\000
	char *ptr = (char *) malloc(arrsize + 1);
	// Copy line number into buffer
	snprintf(ptr, arrsize+1, "%d", line);
		
	err_struct.filename = strdup(filename);
	err_struct.functname = strdup(functname);
	err_struct.line = ptr;
	// The code member is determined by the error_code member
	err_struct.code = strdup(error_code_string[err]);
}

#endif
