#ifndef STREAM_HANDLER_H
#define STREAM_HANDLER_H
//#include <stdlib.h>
//#include <stdio.h>
//#include <stdarg.h>

enum stream_errors {
	null_handler = -1,
	null_filename = -2,
	null_mode = -3,
	fopen_fail = -4,
	null_stream = -5,
	fclose_error = -6,
	null_buffer = -7,
	null_file_mode = -8
};

typedef struct stream_handler {
	FILE *stream;
	char *filename;
	char *mode;
	void (*delete_stream_handler)(struct stream_handler *);
	int (*write)(struct stream_handler *, char *);
	int (*var_write)(struct stream_handler *, char *, ...);
	int (*read)(struct stream_handler *, char *, size_t);
	int (*new_stream)(struct stream_handler *, char *, char *);
} stream_handler;

stream_handler *new_stream_handler(char *, char *);
void delete_stream_handler(stream_handler *);
int write(stream_handler *, char *);
int var_write(stream_handler *, char *, ...);
int read(stream_handler *, char *, size_t);
int new_stream(stream_handler *, char *, char *);

#endif
