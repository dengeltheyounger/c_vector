#ifndef	ERROR_H
#define	ERROR_H

/* The reason why I use this macro is because I intend to use the
 * __attribute__ macro in order to have it deallocate memory as soon 
 * this goes out of scope (in other words, before the program ends).
 */
#ifdef __GNUC__
	#define	\
	typedef struct error_info {	\
		char *filename;	\
		char *functname;	\
		char *line;	\
		char *code;	\
	} error_info;	\
	\
	error_info err_struct;	\
	/* This will set the error info struct. It will handle allocation */	\
	/* and deallocation */	\
	void set_error_info(char *, char *, char *, error_code);	\
	__attribute__(destructor ()) void free_error_info();
#endif

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
} error_code;

error_code err;

#endif
