#include "error.h"
	
static inline void free_error_info() {
	if (err_struct.filename != NULL) {
		free(err_struct.filename);
	}
	if (err_struct.functname != NULL) {
		free(err_struct.filename);
	}
	if (err_struct.line != NULL) {
		free(err_struct.line);
	}
	if (err_struct.code != NULL) {
		free(err_struct.code);
	}
}

static inline void set_error_info(char *filename, char *functname, char *line) {
	free_error_info();
		
	err_struct.filename = strdup(filename);
	err_struct.functname = strdup(functname);
	err_struct.line = strdup(line);
	// The code member is determined by the error_code member
	err_struct.code = strdup(error_code_string[err]);
}
