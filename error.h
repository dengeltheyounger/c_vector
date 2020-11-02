#ifndef	ERROR_H
#define	ERROR_H
#ifndef __cplusplus
#include <stdlib.h>
#include <string.h>
#else
#include <cstdlib>
#include <cstring>
#endif

/* The reason why I use this macro is because I intend to use the
 * __attribute__ macro in order to have it deallocate memory as soon 
 * this goes out of scope (in other words, before the program ends).
 */
#ifndef __GNUC__
	// If we're not using gcc, the programmer will need to manually deallocate
	static inline void free_error_info();
#else
	static inline void free_error_info() __attribute__((destructor));
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

const char *error_code_string[] = {
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

static inline void set_error_info(char *filename, char *functname, char *line);

#endif
