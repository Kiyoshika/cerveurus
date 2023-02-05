#ifndef SQLCERVEURUS_H
#define SQLCERVEURUS_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// read a SQL file and return the (heap-allocated) character buffer as a pointer
char* sql_read(const char* const filename);

// fetch SQL from a file and find & replace text.
// find_text is a NULL-terminated array of strings to find.
// replace_text is a NULL-terminated array of strings to replace.
char* sql_find_and_replace(
		const char* const filename,
		const char** const find_text,
		const char** const replace_text);

#endif
