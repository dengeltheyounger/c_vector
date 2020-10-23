#include "stream_handler.h"

// open_set_file is so named because it not only opens the file but also sets the stream in handler
int open_set_file(stream_handler *handler) {
	
	// The stream should exist, the filename and mode should also be defined
	if (handler == NULL) {
		return null_handler;
	}
	
	else if (handler->filename == NULL) {
		return null_filename;
	}
	
	else if (handler->mode == NULL) {
		return null_mode;
	}
	
	FILE *result;
	
	result = fopen(handler->filename, handler->mode);
	
	if (result == NULL) {
		return fopen_fail;
	}
	
	handler->stream = result;
	
	return 0;
}

int close_set_file(stream_handler *handler) {
	
	// make sure that both stream_handler and the stream exist
	if (handler == NULL) {
		return null_handler;
	}
	
	else if (handler->stream == NULL) {
		return null_stream;
	}
	
	int result = fclose(handler->stream);
	
	if (result == EOF) {
		return fclose_error;
	}
	
	handler->stream = NULL;
	
	return 0;
}	

int write(stream_handler *handler, char *towrite) {
	
	// As always, make sure that handler has actually been created
	if (handler == NULL) {
		return null_handler;
	}
	
	// Make sure that stream has been created too
	else if (handler->stream == NULL) {
		return null_stream;
	}
	
	// write to file
	// fprintf returns negative number on error. Let caller handle that
	int result = fprintf(handler->stream, towrite);
	
	if (result < 0) {
		return result;
	}
	
	return 0;
}

int var_write(stream_handler *handler, char *format, ...) {
	// handler must have been created
	if (handler == NULL) {
		return null_handler;
	}
	
	//stream must have been opened
	if (handler->stream == NULL) {
		return null_stream;
	}
	
	va_list args;
	
	va_start(args, format);
	
	// vfprintf returns a negative number on error. Let caller handle that
	int result = vfprintf(handler->stream, format, args);
	
	va_end(args);
	
	if (result < 0) {
		return result;
	}
	
	return 0;
}

long int file_size(FILE *ptr) {
	fseek(ptr, 0, SEEK_END);
	int size = ftell(ptr);
	fseek(ptr, 0, SEEK_SET);
	
	return size;
}	

int read(stream_handler *handler, char *buffer, size_t buffersize) {

	if (handler == NULL) {
		return null_handler;
	}
	
	if (handler->stream == NULL) {
		return null_stream;
	}
	
	if (buffer == NULL) {
		return null_buffer;
	}
	
	int result = fread(buffer, 1, buffersize, handler->stream);
	
	if (result <= buffersize) {
		int checkeof = feof(handler->stream);
		
		// If eof was returned, then return the file size instead
		if (checkeof != 0) {
			return file_size(handler->stream);
		}
		
		int checkerror = ferror(handler->stream);
		
		if (checkerror != 0) {
			return checkerror;
		}
	}
	
	// If eof was not read, then return the number of characters read
	return result;
}

int new_stream(stream_handler *handler, char *filename, char *mode) {
	int closeresult, openresult;
		
	if (handler == NULL) {
		return null_handler;
	}
	
	// Either the filename or the mode must be passed
	if (filename == NULL && mode == NULL) {
		return null_file_mode;
	}
	
	// if filename is null, then previous filename will be assumed
	else if (filename == NULL) {
		// Make sure that stream_handler has a file name before proceeding
		if (handler->filename == NULL) {
			return null_filename;
		}
		
		// close the file stream if it is opened
		if (handler->stream != NULL) {
			int closeresult = close_set_file(handler);
			
			// Return error if there was trouble closing
			if (closeresult != 0) {
				return closeresult;
			}
		}
			
		handler->mode = mode;
		// open a new file and set the struct. Return the result
		openresult = open_set_file(handler);
		return openresult;
	}
	
	// if mode is null, then previous mode will be assumed
	else if (mode == NULL) {
		
		// Make sure that a mode has been set in handler
		if (handler->mode == NULL) {
			return null_mode;
		}
		
		// Check if the handler's stream is already opened
		if (handler->stream != NULL) {
			// Close the handler's stream if so
			closeresult = close_set_file(handler);
		
			// Return error if there was trouble closing
			if (closeresult != 0) {
				return closeresult;
			}
		}
		
		handler->filename = filename;
		// open a new file and set the struct. Return the result
		openresult = open_set_file(handler);
		
		return openresult;
	}
	
	return 0;
}	

void set_funct_ptr(stream_handler *stream) {
	stream->delete_stream_handler = &delete_stream_handler;
	stream->write = &write;
	stream->var_write = &var_write;
	stream->read = &read;
	stream->new_stream = &new_stream;
}

stream_handler *new_stream_handler(char *filename, char *mode) {
	int result;
	stream_handler *handler = (stream_handler*) malloc(sizeof(stream_handler));
	
	// Set stream to null. This indicates to the destructor that no file is open
	handler->stream = NULL;
		
	if (handler == NULL) {
		return NULL;
	}
	
	set_funct_ptr(handler);

	// If both filename and mode are not null, open a file descriptor and set
	if (filename != NULL && mode != NULL) {
		handler->filename = filename;
		handler->mode = mode;
		
		result = open_set_file(handler);
		
		if (result == fopen_fail) {
			fprintf(stderr, "Error creating opening file for stream_handler. Deleting stream_handler.\n");
			delete_stream_handler(handler);
			
			return NULL;
		}
		
		return handler;
	}
	
	// If either is not present, set filename or mode in struct and then return
	// The one not provided will be set to null
	else if (filename != NULL) {
		handler->filename = filename;
		handler->mode = NULL;
	}
	
	else if (mode != NULL) {
		handler->mode = mode;
		handler->filename = NULL;
	}
	
	else if (filename == NULL && mode == NULL) {
		handler->mode = NULL;
		handler->filename = NULL;
	}
	
	return handler;
}

void delete_stream_handler(stream_handler *handler) {
	
	if (handler->stream != NULL) {
		close_set_file(handler);
	}
	
	if (handler != NULL) {
		free(handler);
	}
}
