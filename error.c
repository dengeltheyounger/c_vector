#include "error.h"

/* As of right now, set_error_info will only set the error struct if a
 * GNU compiler is used. This is because set_error_info dynamically allocates
 * and deallocates as needed. If a GNU compiler is not used, then set error
 * is intended to act as a black hole and the user should really on the 
 * error code. Granted, if there is a better way to handle this, we should
 * definitely use the better method. The nice thing is that we have a basic
 * api defined. c_vector and c_map can focus on the api without worrying
 * about how exactly it is implemented underneath.
 */
#ifdef __GNUC__
	static inline void set_error_info(char *filename, char *functname, char *line, error_code code) {
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
		
		err_struct.filename = strdup(filename);
		err_struct.functname = strdup(functname);
		err_struct.line = strdup(line);
		err_struct.code = strdup(error_code_string[code]);
	}
	
	void free_error_info() {
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
#else
	static inline void set_error_info(char *filename, char *functname, char *line, error_code code) {
		return;
	}
#endif
